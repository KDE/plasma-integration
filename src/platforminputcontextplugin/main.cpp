/*
 *  Copyright 2020 Carson Black <uhhadd@gmail.com>
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

#include <QObject>
#include <QtPlugin>
#include <qpa/qplatforminputcontextplugin_p.h>

#include "plasmaimcontext.h"

QT_BEGIN_NAMESPACE

class PlasmaIM : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "plasmaim.json")

public:
    QPlatformInputContext *create(const QString&, const QStringList&) Q_DECL_OVERRIDE;
};

QPlatformInputContext *PlasmaIM::create(const QString& system, const QStringList&)
{
    if (system == "plasmaim") {
        return new PlasmaIMContext;
    }

    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
