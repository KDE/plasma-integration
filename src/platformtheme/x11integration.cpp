/*  This file is part of the KDE libraries
 *  Copyright 2015 Martin Gräßlin <mgraesslin@kde.org>
 *  Copyright 2016 Marco Martin <mart@kde.org>
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
#include "x11integration.h"

#include <QCoreApplication>
#include <QX11Info>
#include <QWindow>
#include <NETWM>

X11Integration::X11Integration()
    : QObject()
{
}

X11Integration::~X11Integration() = default;

void X11Integration::init()
{
    QCoreApplication::instance()->installEventFilter(this);
}

bool X11Integration::eventFilter(QObject *watched, QEvent *event)
{
    //the drag and drop window should NOT be a tooltip
    //https://bugreports.qt.io/browse/QTBUG-52560
    if (event->type() == QEvent::Show && watched->inherits("QShapedPixmapWindow")) {
        //static cast should be safe there
        QWindow *w = static_cast<QWindow *>(watched);
        NETWinInfo info(QX11Info::connection(), w->winId(), QX11Info::appRootWindow(), NET::WMWindowType, NET::Properties2());
        info.setWindowType(NET::DNDIcon);
        // TODO: does this flash the xcb connection?
    }
    return false;
}

