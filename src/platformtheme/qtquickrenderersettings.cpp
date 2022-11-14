/*
    SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2020 Piotr Henryk Dabrowski <phd@phd.re>
    SPDX-FileCopyrightText: 2021 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "renderersettings.h"

#include <QGuiApplication>
#include <QLibraryInfo>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include <QVersionNumber>

/**
 * If QtQuick is configured (QQuickWindow::sceneGraphBackend()) to use the OpenGL backend,
 * check if it is supported or otherwise reconfigure QtQuick to fallback to software mode.
 * This function is called by init().
 *
 * @returns true if the selected backend is supported, false on fallback to software mode.
 */
static bool checkBackend(QOpenGLContext &checkContext)
{
    if (!QQuickWindow::sceneGraphBackend().isEmpty()) {
        return true; // QtQuick is not configured to use the OpenGL backend
    }

    // kwin wayland has it's own QPA, it is unable to create a GL context at this point.
    // KF6 TODO, drop this . The issue will be resolved in future kwin releases.
    QString platformName = qApp->platformName();
    if (platformName == QLatin1String("wayland-org.kde.kwin.qpa")) {
        return true;
    }

#ifdef QT_NO_OPENGL
    bool ok = false;
#else
    bool ok = checkContext.create();
#endif
    if (!ok) {
        qWarning("Warning: fallback to QtQuick software backend.");
        QQuickWindow::setSceneGraphBackend(QStringLiteral("software"));
    }
    return ok;
}

void initializeRendererSessions()
{
    // This is loaded via Q_COREAPP_STARTUP_FUNCTION

    // Due to a quirk this gets called twice, see QTBUG-54479

    // The order of events is:
    // Q*Application constructor starts
    // We load the QPA
    // We load the QPT (The first arguably incorrect invocation triggers)
    // QPA gets initalised
    // QCoreApplication constructor ends
    // Second (correct) invocation
    // it's important that we run after the QPA is initalised'

    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;
        return;
    }

    PlasmaQtQuickSettings::RendererSettings s;
    QOpenGLContext checkContext;
    if (!s.sceneGraphBackend().isEmpty()) {
        QQuickWindow::setSceneGraphBackend(s.sceneGraphBackend());
    } else {
        QQuickWindow::setSceneGraphBackend(QStringLiteral(""));
        checkBackend(checkContext);
    }

    if (!qEnvironmentVariableIsSet("QSG_RENDER_LOOP")) {
        if (!s.renderLoop().isEmpty()) {
            qputenv("QSG_RENDER_LOOP", s.renderLoop().toLatin1());
        } else if (QGuiApplication::platformName() == QLatin1String("wayland")) {
#if QT_CONFIG(opengl)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            // qtwayland can't ensure that wl_surface lives as long as the qtquick render
            // thread needs it. If the wl_surface is destroyed while the render thread is
            // still active, the compositor will eventually post a protocol error and the
            // client (us) will crash. This is fixed in Qt 6, but there are no suitable
            // alternatives in Qt 5, other than to force basic render loop.
            //
            // Note that the NVIDIA case below is still valid and should be kept in Qt 6!
            qputenv("QSG_RENDER_LOOP", "basic");
#else
            // Workaround for Bug 432062 / QTBUG-95817
            QOffscreenSurface surface;
            surface.create();
            if (checkContext.makeCurrent(&surface)) {
                const char *vendor = reinterpret_cast<const char *>(checkContext.functions()->glGetString(GL_VENDOR));
                if (qstrcmp(vendor, "NVIDIA Corporation") == 0) {
                    // Otherwise Qt Quick Windows break when resized
                    qputenv("QSG_RENDER_LOOP", "basic");
                }
            }
#endif
#endif
        }
    }
}

// Because this file gets loaded in the platform plugin, the QGuiApplication already exists
Q_COREAPP_STARTUP_FUNCTION(initializeRendererSessions)
