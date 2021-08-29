/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef KWAYLANDINTEGRATION_H
#define KWAYLANDINTEGRATION_H

#include <QHash>
#include <QObject>

class QWindow;

namespace KWayland
{
namespace Client
{
class ServerSideDecorationPaletteManager;
class AppMenuManager;
class Registry;
}
}

class KWaylandIntegration : public QObject
{
    Q_OBJECT
public:
    explicit KWaylandIntegration();
    ~KWaylandIntegration() override;
    void init();

    void setAppMenu(QWindow *window, const QString &serviceName, const QString &objectPath);
    void setPalette(QWindow *window, const QString &paletteName);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void shellSurfaceCreated(QWindow *w);
    void shellSurfaceDestroyed(QWindow *w);

    void installColorScheme(QWindow *w);
    KWayland::Client::AppMenuManager *m_appMenuManager = nullptr;
    KWayland::Client::ServerSideDecorationPaletteManager *m_paletteManager = nullptr;
    KWayland::Client::Registry *m_registry = nullptr;

    struct WindowInfo {
        QString appMenuServiceName;
        QString appMenuObjectPath;
    };
    QHash<QWindow *, WindowInfo> m_windowInfo;
};

#endif
