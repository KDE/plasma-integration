/*
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    QPlatformInputContext *create(const QString &, const QStringList &) Q_DECL_OVERRIDE;
};

QPlatformInputContext *PlasmaIM::create(const QString &system, const QStringList &)
{
    if (system == "plasmaim") {
        return new PlasmaIMContext;
    }

    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
