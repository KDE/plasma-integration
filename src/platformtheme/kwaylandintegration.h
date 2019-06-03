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
#ifndef KWAYLANDINTEGRATION_H
#define KWAYLANDINTEGRATION_H

#include <QObject>
#include <QHash>

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
    KWayland::Client::Registry* m_registry = nullptr;

    struct WindowInfo {
        QString appMenuServiceName;
        QString appMenuObjectPath;
    };
    QHash<QWindow*, WindowInfo> m_windowInfo;
};

#endif
