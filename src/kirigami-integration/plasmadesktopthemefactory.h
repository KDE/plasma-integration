/*
*   Copyright (C) 2017 by Marco Martin <mart@kde.org>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Library General Public License for more details
*
*   You should have received a copy of the GNU Library General Public
*   License along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef PLASMADESKTOPTHEMEFACTORY_H
#define PLASMADESKTOPTHEMEFACTORY_H

#include <Kirigami2/PlatformThemeFactory>
#include <QObject>

class PlasmaDesktopThemeFactory : public Kirigami::PlatformThemeFactory
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.kde.kirigami.PlatformThemeFactory" FILE "kirigamiplasmaintegration.json")

    Q_INTERFACES(Kirigami::PlatformThemeFactory)

public:
    explicit PlasmaDesktopThemeFactory(QObject *parent = nullptr);
    ~PlasmaDesktopThemeFactory();

    Kirigami::PlatformTheme *create(QObject *parent) Q_DECL_OVERRIDE;
};



#endif // PLASMADESKTOPTHEMEFACTORY_H
