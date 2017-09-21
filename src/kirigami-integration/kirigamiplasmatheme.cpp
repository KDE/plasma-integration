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

#include "kirigamiplasmatheme.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include <QPalette>
#include <QDebug>
#include <QQuickWindow>
#include <QTimer>

#include <KColorScheme>


PlasmaTheme::PlasmaTheme(QObject *parent)
    : PlatformTheme(parent)
{
    //TODO: correct?
    connect(qApp, &QGuiApplication::fontDatabaseChanged, this, [this]() {setDefaultFont(qApp->font());});

    connect(this, &PlasmaTheme::colorSetChanged,
            this, &PlasmaTheme::syncColors);
    syncColors();
}

PlasmaTheme::~PlasmaTheme()
{
}

QStringList PlasmaTheme::keys() const
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

void PlasmaTheme::syncColors()
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

QColor PlasmaTheme::buttonTextColor() const
{
    return m_buttonTextColor;
}

QColor PlasmaTheme::buttonBackgroundColor() const
{
    return m_buttonBackgroundColor;
}

QColor PlasmaTheme::buttonHoverColor() const
{
    return m_buttonHoverColor;
}

QColor PlasmaTheme::buttonFocusColor() const
{
    return m_buttonFocusColor;
}


QColor PlasmaTheme::viewTextColor() const
{
    return m_viewTextColor;
}

QColor PlasmaTheme::viewBackgroundColor() const
{
    return m_viewBackgroundColor;
}

QColor PlasmaTheme::viewHoverColor() const
{
    return m_viewHoverColor;
}

QColor PlasmaTheme::viewFocusColor() const
{
    return m_viewFocusColor;
}

#include "moc_kirigamiplasmatheme.cpp"
