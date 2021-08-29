/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "kwaylandintegration.h"

#include <QExposeEvent>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

#include <KWayland/Client/appmenu.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/server_decoration.h>
#include <KWayland/Client/server_decoration_palette.h>
#include <KWayland/Client/surface.h>
#include <KWindowEffects>

using namespace KWayland::Client;

static const QByteArray s_schemePropertyName = QByteArrayLiteral("KDE_COLOR_SCHEME_PATH");
static const QByteArray s_blurBehindPropertyName = QByteArrayLiteral("ENABLE_BLUR_BEHIND_HINT");

KWaylandIntegration::KWaylandIntegration()
    : QObject()
{
}

KWaylandIntegration::~KWaylandIntegration() = default;

void KWaylandIntegration::init()
{
    auto connection = ConnectionThread::fromApplication(this);
    if (!connection) {
        return;
    }
    m_registry = new Registry(this);
    m_registry->create(connection);
    QObject::connect(m_registry, &Registry::interfacesAnnounced, this, [this] {
        QCoreApplication::instance()->installEventFilter(this);
        const auto menuInterface = m_registry->interface(Registry::Interface::AppMenu);
        if (menuInterface.name != 0) {
            m_appMenuManager = m_registry->createAppMenuManager(menuInterface.name, menuInterface.version, this);
        }
    });

    m_registry->setup();
}

bool KWaylandIntegration::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Expose) {
        auto ee = static_cast<QExposeEvent *>(event);
        if (ee->region().isNull()) {
            return false;
        }
        QWindow *w = qobject_cast<QWindow *>(watched);
        if (!w || w->parent() || !w->isVisible()) {
            return false;
        }
        if (w->property("org.kde.plasma.integration.shellSurfaceCreated").isNull()) {
            shellSurfaceCreated(w);
        }
    } else if (event->type() == QEvent::Hide) {
        QWindow *w = qobject_cast<QWindow *>(watched);
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
        KWindowEffects::enableBlurBehind(w, blurBehindProperty.toBool());
    }
    // create deco
    Surface *s = Surface::fromWindow(w);
    if (!s) {
        return;
    }

    w->setProperty("org.kde.plasma.integration.shellSurfaceCreated", true);

    if (m_appMenuManager) {
        auto menu = m_appMenuManager->create(s, w);
        w->setProperty("org.kde.plasma.integration.appmenu", QVariant::fromValue(menu));
        menu->setAddress(m_windowInfo[w].appMenuServiceName, m_windowInfo[w].appMenuObjectPath);
    }
}

void KWaylandIntegration::shellSurfaceDestroyed(QWindow *w)
{
    w->setProperty("org.kde.plasma.integration.shellSurfaceCreated", QVariant());

    delete w->property("org.kde.plasma.integration.appmenu").value<AppMenu *>();
    w->setProperty("org.kde.plasma.integration.appmenu", QVariant());

    delete w->property("org.kde.plasma.integration.palette").value<ServerSideDecorationPalette *>();
    w->setProperty("org.kde.plasma.integration.palette", QVariant());
}

void KWaylandIntegration::installColorScheme(QWindow *w)
{
    if (!m_paletteManager) {
        const auto paletteManagerInterface = m_registry->interface(Registry::Interface::ServerSideDecorationPalette);
        if (paletteManagerInterface.name == 0) {
            return;
        } else {
            m_paletteManager = m_registry->createServerSideDecorationPaletteManager(paletteManagerInterface.name, paletteManagerInterface.version, this);
        }
    }
    auto palette = w->property("org.kde.plasma.integration.palette").value<ServerSideDecorationPalette *>();
    if (!palette) {
        Surface *s = Surface::fromWindow(w);
        if (!s) {
            return;
        }
        palette = m_paletteManager->create(s, w);
        w->setProperty("org.kde.plasma.integration.palette", QVariant::fromValue(palette));
    }
    if (palette) {
        palette->setPalette(qApp->property(s_schemePropertyName.constData()).toString());
    }
}

void KWaylandIntegration::setAppMenu(QWindow *window, const QString &serviceName, const QString &objectPath)
{
    if (!m_windowInfo.contains(window)) { // effectively makes this connect unique
        connect(window, &QObject::destroyed, this, [=]() {
            m_windowInfo.remove(window);
        });
    }
    m_windowInfo[window].appMenuServiceName = serviceName;
    m_windowInfo[window].appMenuObjectPath = objectPath;
    auto menu = window->property("org.kde.plasma.integration.appmenu").value<AppMenu *>();
    if (menu) {
        menu->setAddress(serviceName, objectPath);
    }
}
