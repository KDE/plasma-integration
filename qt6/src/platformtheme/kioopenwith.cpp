// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "kioopenwith.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QWidget>
#include <private/qgenericunixservices_p.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <KJob>
#include <KJobWidgets>

namespace
{
QString desktopPortalService()
{
    return QStringLiteral("org.freedesktop.impl.portal.desktop.kde");
}

QString desktopPortalPath()
{
    return QStringLiteral("/org/freedesktop/portal/desktop");
}
} // namespace

KIOOpenWith::KIOOpenWith(QWidget *parentWidget, QObject *parent)
    : KIO::OpenWithHandlerInterface(parent)
    , m_parentWidget(parentWidget)
{
}

void KIOOpenWith::promptUserForApplication(KJob *job, const QList<QUrl> &urls, const QString &mimeType)
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
        if (auto unixServices = dynamic_cast<QGenericUnixServices *>(services)) {
            windowId = unixServices->portalWindowIdentifier(widget->window()->windowHandle());
        }
    }

    QDBusMessage message = QDBusMessage::createMethodCall(desktopPortalService(),
                                                          desktopPortalPath(),
                                                          QStringLiteral("org.freedesktop.impl.portal.AppChooser"),
                                                          QStringLiteral("ChooseApplicationPrivate"));

    QStringList urlStrings;
    for (const auto &url : urls) {
        urlStrings << url.toString();
    }
    message << windowId << urlStrings << QVariantMap{{QStringLiteral("ask"), true}};

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message, std::numeric_limits<int>::max());
    auto watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();

        QDBusPendingReply<uint, QVariantMap> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Couldn't get reply";
            qWarning() << "Error: " << reply.error().message();
            Q_EMIT canceled();
        } else {
            if (reply.argumentAt<0>() == 0) {
                Q_EMIT serviceSelected(KService::serviceByDesktopName(reply.argumentAt<1>().value(QStringLiteral("choice")).toString()));
            } else {
                Q_EMIT canceled();
            }
        }
    });
}
