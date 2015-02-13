/*  This file is part of the KDE libraries
 *  Copyright 2013 Kevin Ottens <ervin+bluesystems@kde.org>
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

#include <qpa/qplatformthemeplugin.h>

#include "kdeplatformtheme.h"

#include <config-platformtheme.h>
#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 1))
#if HAVE_X11
#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QX11Info>
#include <xcb/xcb.h>
#endif
#endif

class KdePlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "kdeplatformtheme.json")
public:
    KdePlatformThemePlugin(QObject *parent = 0)
        : QPlatformThemePlugin(parent) {}

    QPlatformTheme *create(const QString &key, const QStringList &paramList) Q_DECL_OVERRIDE
    {
        Q_UNUSED(key)
        Q_UNUSED(paramList)
#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 1))
        // Must be done after we have an event-dispatcher. By posting a method invocation
        // we are sure that by the time the method is called we have an event-dispatcher.
        QMetaObject::invokeMethod(this, "setupXcbFlush", Qt::QueuedConnection);
#endif
        return new KdePlatformTheme;
    }

public Q_SLOTS:
    void setupXcbFlush();
};

void KdePlatformThemePlugin::setupXcbFlush()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 1))
#if HAVE_X11
    // this is a workaround for BUG 334858
    // it ensures that the xcb connection gets flushed before the EventDispatcher
    // is going to block. Qt does not guarantee this in all cases.
    // For Qt this issue is addressed in https://codereview.qt-project.org/85654
    // TODO: remove again once we depend on a Qt version with the patch.
    if (!QX11Info::isPlatformX11() || qstrcmp(qVersion(), "5.3.1") >= 0) {
        return;
    }
    connect(QCoreApplication::eventDispatcher(), &QAbstractEventDispatcher::aboutToBlock,
        []() {
            xcb_flush(QX11Info::connection());
        }
    );
#endif
#endif
}

#include "main.moc"
