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

#include <QCoreApplication>
#include <QPlatformSurfaceEvent>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>
#include <KWayland/Client/server_decoration.h>

using namespace KWayland::Client;

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
        }
    );

    registry->setup();
    connection->roundtrip();
}

bool KWaylandIntegration::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_decoration) {
        return false;
    }
    if (event->type() == QEvent::PlatformSurface) {
        QWindow *w = qobject_cast<QWindow*>(watched);
        if (!w || w->parent()) {
            return false;
        }
        if (auto e = dynamic_cast<QPlatformSurfaceEvent*>(event)) {
            switch (e->surfaceEventType()) {
            case QPlatformSurfaceEvent::SurfaceCreated: {
                Surface *s = Surface::fromWindow(w);
                if (!s) {
                    return false;
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
                break;
            }
            case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: {
                delete w->property("org.kde.plasma.integration.waylandserverdecoration").value<ServerSideDecoration*>();
                break;
            }
            default:
                // nothing
                break;
            }
        }
    }
    return false;
}
