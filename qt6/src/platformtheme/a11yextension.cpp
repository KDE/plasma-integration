// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "a11yextension.h"

#include <QAccessibleInterface>
#include <QDBusConnectionInterface>
#include <QGuiApplication>
#include <QUuid>
#include <QWindow>

using namespace Qt::StringLiterals;

A11yExtension::A11yExtension()
    : m_connection([]() -> auto {
        if (const auto address = qgetenv("AT_SPI_BUS_ADDRESS"); !address.isEmpty()) {
            return QDBusConnection(QDBusConnection::connectToBus(address, "a11y"_L1));
        }

        if (auto connection = QDBusConnection::sessionBus(); connection.interface()->isServiceRegistered("org.a11y.Bus"_L1)) {
            auto method = QDBusMessage::createMethodCall(QStringLiteral("org.a11y.Bus"),
                                                         QStringLiteral("/org/a11y/bus"),
                                                         QStringLiteral("org.a11y.Bus"),
                                                         QStringLiteral("GetAddress"));
            // The a11y extension is opt-in for selenium, we can afford to deploy a blocking call here because of that and
            // save ourselves a race condition negotiation.
            QDBusReply<QString> reply = connection.call(method);
            if (reply.isValid()) {
                return QDBusConnection(QDBusConnection::connectToBus(reply.value(), "a11y"_L1));
            }
        }

        return QDBusConnection::connectToBus(u"unix:path=%1/at-spi/bus_1"_s.arg(qEnvironmentVariable("XDG_RUNTIME_DIR")), u"a11y"_s);
    }())
{
    if (!m_connection.isConnected()) {
        qWarning() << "A11yExtension failed to connect to at-spi bus";
        return;
    }
    m_connection.registerObject("/org/kde/plasma/a11y/atspi/extension", this, QDBusConnection::ExportAllSlots);
}

QVariantHash A11yExtension::accessibleProperties(const QString &accessibleId)
{
    const auto iface = QAccessible::accessibleInterface(accessibleId.toUInt());
    const auto rect = iface->rect();
    return QVariantHash{
        {u"rect.x"_s, rect.x()},
        {u"rect.y"_s, rect.y()},
        {u"rect.width"_s, rect.width()},
        {u"rect.height"_s, rect.height()},
    };
}

QString A11yExtension::identifyWindowByTitle(const QString &accessibleId)
{
    const auto iface = QAccessible::accessibleInterface(accessibleId.toUInt());
    const auto window = iface->window();
    const auto uuid = QUuid::createUuid();
    const auto uuidString = uuid.toString();
    m_originalTitles.insert(window, window->title());
    window->setTitle(uuidString);
    return uuidString;
}

void A11yExtension::resetWindowTitle(const QString &uuid)
{
    for (const auto &[window, caption] : m_originalTitles.asKeyValueRange()) {
        if (window->title().startsWith(uuid)) {
            window->setTitle(caption);
            m_originalTitles.remove(window); // WARNING: breaks iterator return immediately!
            return;
        }
    }
}
