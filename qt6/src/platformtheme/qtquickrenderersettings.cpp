/*
    SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2020 Piotr Henryk Dabrowski <phd@phd.re>
    SPDX-FileCopyrightText: 2021 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "renderersettings.h"

#include <QGuiApplication>
#include <QQuickWindow>

void initializeRendererSessions()
{
    PlasmaQtQuickSettings::RendererSettings s;

    QSGRendererInterface::GraphicsApi graphicsApi = QSGRendererInterface::Unknown;

    switch (s.sceneGraphBackend()) {
    case PlasmaQtQuickSettings::RendererSettings::software:
        graphicsApi = QSGRendererInterface::Software;
        break;
    case PlasmaQtQuickSettings::RendererSettings::opengl:
        graphicsApi = QSGRendererInterface::OpenGL;
        break;
    case PlasmaQtQuickSettings::RendererSettings::vulkan:
        graphicsApi = QSGRendererInterface::Vulkan;
        break;
    default:
        break;
    }

    if (graphicsApi != QSGRendererInterface::Unknown) {
        QQuickWindow::setGraphicsApi(graphicsApi);
    }

    if (!qEnvironmentVariableIsSet("QSG_RENDER_LOOP")) {
        if (s.renderLoop() == PlasmaQtQuickSettings::RendererSettings::basic) {
            qputenv("QSG_RENDER_LOOP", "basic");
        }
    }
}

// Because this file gets loaded in the platform plugin, the QGuiApplication already exists
Q_COREAPP_STARTUP_FUNCTION(initializeRendererSessions)
