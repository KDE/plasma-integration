// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022-2025 Harald Sitter <sitter@kde.org>

#include "kioopenwithxdp.h"

#include <fcntl.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QWidget>
#include <private/qdesktopunixservices_p.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <KJob>
#include <KJobWidgets>

using namespace Qt::StringLiterals;

KIOOpenWithXDP::KIOOpenWithXDP(QWidget *parentWidget, QObject *parent)
    : KIO::OpenWithHandlerInterface(parent)
    , m_parentWidget(parentWidget)
{
}

void KIOOpenWithXDP::promptUserForApplication(KJob *job, const QList<QUrl> &urls, const QString &mimeType)
{
    Q_UNUSED(mimeType);

    QWidget *widget = nullptr;
    if (job) {
        widget = KJobWidgets::window(job);
    }

    if (!widget) {
        widget = m_parentWidget;
    }

    QString windowId;
    if (widget) {
        widget->window()->winId(); // ensure we have a handle so we can export a window (without this windowHandle() may be null)
        auto services = QGuiApplicationPrivate::platformIntegration()->services();
        if (auto unixServices = dynamic_cast<QDesktopUnixServices *>(services)) {
            windowId = unixServices->portalWindowIdentifier(widget->window()->windowHandle());
        }
    }

    for (const QUrl &url : urls) {
        const auto message = [&] {
            if (url.scheme() == "file"_L1) {
                QDBusMessage message = QDBusMessage::createMethodCall(u"org.freedesktop.portal.Desktop"_s,
                                                                      u"/org/freedesktop/portal/desktop"_s,
                                                                      u"org.freedesktop.portal.OpenURI"_s,
                                                                      u"OpenFile"_s);

                bool writable = true;
                auto fd = open(url.toLocalFile().toUtf8().constData(), O_RDWR | O_CLOEXEC);
                if (fd < 0) { // try read only
                    fd = open(url.toLocalFile().toUtf8().constData(), O_RDONLY | O_CLOEXEC);
                    writable = false;
                    if (fd < 0) { // give up
                        qWarning() << "Could not open file to read or readwrite for portal openwith:" << url;
                        return QDBusMessage();
                    }
                }
                QDBusUnixFileDescriptor dbusFd;
                dbusFd.giveFileDescriptor(fd);

                message << windowId //
                        << QVariant::fromValue(dbusFd) //
                        << QVariantMap{
                               {QStringLiteral("ask"), true}, //
                               {QStringLiteral("writable"), writable}, //
                           };

                return message;
            }

            QDBusMessage message = QDBusMessage::createMethodCall(u"org.freedesktop.portal.Desktop"_s,
                                                                  u"/org/freedesktop/portal/desktop"_s,
                                                                  u"org.freedesktop.portal.OpenURI"_s,
                                                                  u"OpenURI"_s);

            message << windowId //
                    << url.toString() //
                    << QVariantMap{
                           {QStringLiteral("ask"), true}, //
                           {QStringLiteral("writable"), true}, //
                       };
            return message;
        }();

        if (message.service().isEmpty()) { // happens when the message construction failed above
            Q_EMIT canceled();
            return;
        }

        QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message, std::numeric_limits<int>::max());
        auto watcher = new QDBusPendingCallWatcher(pendingCall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, message](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusReply<QDBusObjectPath> reply = *watcher;
            if (!reply.isValid()) {
                qWarning() << "Error: " << reply.error().message();
                Q_EMIT canceled();
                return;
            }

            QDBusConnection::sessionBus().connect(u"org.freedesktop.portal.Desktop"_s,
                                                  reply.value().path(),
                                                  u"org.freedesktop.portal.Request"_s,
                                                  u"Response"_s,
                                                  this,
                                                  SLOT(onApplicationChosen(uint, QVariantMap)));
        });
    }
}

void KIOOpenWithXDP::onApplicationChosen(uint responseCode, [[maybe_unused]] const QVariantMap &resultMap)
{
    if (responseCode != 0) {
        Q_EMIT canceled();
        return;
    }
    Q_EMIT handled();
}
