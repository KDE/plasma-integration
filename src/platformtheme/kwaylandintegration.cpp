/*  This file is part of the KDE libraries
 *  Copyright 2015 Martin Gräßlin <mgraesslin@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */
#include "kwaylandintegration.h"

#include <QGuiApplication>
#include <QPlatformSurfaceEvent>
#include <qpa/qplatformnativeinterface.h>

#include <qpa/qplatformnativeinterface.h>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>
#include <KWayland/Client/server_decoration.h>
#include <KWayland/Client/appmenu.h>
#include <KWindowEffects>

using namespace KWayland::Client;

static const QByteArray s_schemePropertyName = QByteArrayLiteral("KDE_COLOR_SCHEME_PATH");
static const QByteArray s_blurBehindPropertyName = QByteArrayLiteral("ENABLE_BLUR_BEHIND_HINT");

KWaylandIntegration::KWaylandIntegration()
    : QObject()
    , m_decoration(Q_NULLPTR)
{
}

KWaylandIntegration::~KWaylandIntegration() = default;

void KWaylandIntegration::init()
{
    auto connection = ConnectionThread::fromApplication(this);
    if (!connection) {
        return;
    }
    Registry *registry = new Registry(this);
    registry->create(connection);
    QObject::connect(registry, &Registry::interfacesAnnounced, this,
        [registry, this] {
            const auto interface = registry->interface(Registry::Interface::ServerSideDecorationManager);
            if (interface.name != 0) {
                m_decoration = registry->createServerSideDecorationManager(interface.name, interface.version, this);
                qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
                QCoreApplication::instance()->installEventFilter(this);

            }
            const auto menuInterface = registry->interface(Registry::Interface::AppMenu);
            if (menuInterface.name != 0) {
                m_appMenuManager = registry->createAppMenuManager(menuInterface.name, menuInterface.version, this);
            }
        }
    );

    registry->setup();
    connection->roundtrip();
}

bool KWaylandIntegration::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_decoration) {
        return false;
    } else if (event->type() == QEvent::Expose) {
        auto ee = static_cast<QExposeEvent*>(event);
        if (ee->region().isNull()) {
            return false;
        }
        QWindow *w = qobject_cast<QWindow*>(watched);
        if (!w || w->parent() || !w->isVisible()) {
            return false;
        }
        if(w->property("org.kde.plasma.integration.waylandserverdecoration").isNull()) {
            shellSurfaceCreated(w);
        }
    } else if (event->type() == QEvent::Hide) {
        QWindow *w = qobject_cast<QWindow*>(watched);
        if (!w || w->parent()) {
            return false;
        }
        shellSurfaceDestroyed(w);
    } else if (event->type() == QEvent::ApplicationPaletteChange) {
        const auto topLevelWindows = QGuiApplication::topLevelWindows();
        for (QWindow *w : topLevelWindows) {
            installColorScheme(w);
        }
    }

    return false;
}

void KWaylandIntegration::shellSurfaceCreated(QWindow *w)
{
    // set colorscheme hint
    if (qApp->property(s_schemePropertyName.constData()).isValid()) {
        installColorScheme(w);
    }
    const auto blurBehindProperty = w->property(s_blurBehindPropertyName.constData());
    if (blurBehindProperty.isValid()) {
        KWindowEffects::enableBlurBehind(w->winId(), blurBehindProperty.toBool());
    }
    // create deco
    Surface *s = Surface::fromWindow(w);
    if (!s) {
        return;
    }
    auto deco = m_decoration->create(s, w);
    connect(deco, &ServerSideDecoration::modeChanged, w,
        [deco, w] {
            const auto flags = w->flags();
            const auto ourMode = (flags.testFlag(Qt::FramelessWindowHint) || flags.testFlag(Qt::Popup) || flags.testFlag(Qt::ToolTip)) ? ServerSideDecoration::Mode::None : ServerSideDecoration::Mode::Server;
            if (deco->mode() != ourMode) {
                deco->requestMode(ourMode);
            }
        }
    );
    const auto flags = w->flags();
    const auto ourMode = (flags.testFlag(Qt::FramelessWindowHint) || flags.testFlag(Qt::Popup) || flags.testFlag(Qt::ToolTip)) ? ServerSideDecoration::Mode::None : ServerSideDecoration::Mode::Server;
    if (deco->defaultMode() != ourMode) {
        deco->requestMode(ourMode);
    }
    w->setProperty("org.kde.plasma.integration.waylandserverdecoration", QVariant::fromValue(deco));

    if (m_appMenuManager) {
        auto menu = m_appMenuManager->create(s, w);
        w->setProperty("org.kde.plasma.integration.appmenu", QVariant::fromValue(menu));
        menu->setAddress(m_appMenuServiceName, m_appMenuObjectPath);
    }
}

void KWaylandIntegration::shellSurfaceDestroyed(QWindow *w)
{
    delete w->property("org.kde.plasma.integration.waylandserverdecoration").value<ServerSideDecoration*>();
    w->setProperty("org.kde.plasma.integration.waylandserverdecoration", QVariant());

    delete w->property("org.kde.plasma.integration.appmenu").value<AppMenu*>();
    w->setProperty("org.kde.plasma.integration.appmenu", QVariant());
}

void KWaylandIntegration::installColorScheme(QWindow *w)
{
    if (QPlatformNativeInterface *native = qApp->platformNativeInterface()) {
        if (QPlatformWindow *pw = w->handle()) {
            native->setWindowProperty(pw, QString::fromUtf8(s_schemePropertyName), qApp->property(s_schemePropertyName.constData()));
        }
    }
}

void KWaylandIntegration::setWindowProperty(QWindow *window, const QByteArray &name, const QByteArray &value)
{
    if (QPlatformNativeInterface *nativeInterface = qApp->platformNativeInterface()) {
        if (QPlatformWindow *platformWindow = window->handle()) {
            nativeInterface->setWindowProperty(platformWindow, QString::fromUtf8(name), QString::fromUtf8(value));
        }
    }
}

void KWaylandIntegration::setAppMenu(const QString &serviceName, const QString &objectPath)
{
    m_appMenuServiceName = serviceName;
    m_appMenuObjectPath = objectPath;
    auto menu = property("org.kde.plasma.integration.appmenu").value<AppMenu*>();
    if (menu) {
        menu->setAddress(serviceName, objectPath);
    }
}

#include "kwaylandintegration.moc"
