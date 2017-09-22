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

#include "plasmadesktoptheme.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include <QPalette>
#include <QDebug>
#include <QQuickWindow>
#include <QTimer>

#include <KColorScheme>


PlasmaDesktopTheme::PlasmaDesktopTheme(QObject *parent)
    : PlatformTheme(parent)
{
    //TODO: correct?
    connect(qApp, &QGuiApplication::fontDatabaseChanged, this, [this]() {setDefaultFont(qApp->font());});

    connect(this, &PlasmaDesktopTheme::colorSetChanged,
            this, &PlasmaDesktopTheme::syncColors);
    syncColors();
}

PlasmaDesktopTheme::~PlasmaDesktopTheme()
{
}

QStringList PlasmaDesktopTheme::keys() const
{
    QStringList props;
    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i) {
        const QString prop = QString::fromUtf8(metaObject()->property(i).name());
        if (prop != QStringLiteral("keys")) {
            props << prop;
        }
    }
    return props;
}

void PlasmaDesktopTheme::syncColors()
{
    KColorScheme::ColorSet set;
qWarning()<<parent()<<colorSet();
    switch (colorSet()) {
    case PlatformTheme::Button:
        set = KColorScheme::ColorSet::Button;
        break;
    case PlatformTheme::View:
        set = KColorScheme::ColorSet::View;
        break;
    case PlatformTheme::Complementary:
        set = KColorScheme::ColorSet::Complementary;
        break;
    case PlatformTheme::Window:
    default:
        set = KColorScheme::ColorSet::Window;
    }
    KColorScheme scheme(QPalette::Active, set);
    setTextColor(scheme.foreground(KColorScheme::NormalText).color());
    setBackgroundColor(scheme.background(KColorScheme::NormalBackground).color());
    emit colorsChanged();
}

QColor PlasmaDesktopTheme::buttonTextColor() const
{
    return m_buttonTextColor;
}

QColor PlasmaDesktopTheme::buttonBackgroundColor() const
{
    return m_buttonBackgroundColor;
}

QColor PlasmaDesktopTheme::buttonHoverColor() const
{
    return m_buttonHoverColor;
}

QColor PlasmaDesktopTheme::buttonFocusColor() const
{
    return m_buttonFocusColor;
}


QColor PlasmaDesktopTheme::viewTextColor() const
{
    return m_viewTextColor;
}

QColor PlasmaDesktopTheme::viewBackgroundColor() const
{
    return m_viewBackgroundColor;
}

QColor PlasmaDesktopTheme::viewHoverColor() const
{
    return m_viewHoverColor;
}

QColor PlasmaDesktopTheme::viewFocusColor() const
{
    return m_viewFocusColor;
}

#include "moc_plasmadesktoptheme.cpp"
