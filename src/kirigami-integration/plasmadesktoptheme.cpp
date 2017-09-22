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
#include <KIconLoader>

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

QIcon PlasmaDesktopTheme::iconFromTheme(const QString &name)
{
    KColorScheme::ColorSet set;

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
    KIconLoader::global()->setColorSet(set);

    return KDE::icon(name, KIconLoader::global());
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

    const KColorScheme selectionScheme(QPalette::Active, KColorScheme::ColorSet::Selection);
    const KColorScheme scheme(QPalette::Active, set);
    setTextColor(scheme.foreground(KColorScheme::NormalText).color());
    setBackgroundColor(scheme.background(KColorScheme::NormalBackground).color());
    setDisabledTextColor(scheme.foreground(KColorScheme::InactiveText).color());

    setHighlightColor(selectionScheme.background(KColorScheme::NormalBackground).color());
    setHighlightedTextColor(selectionScheme.foreground(KColorScheme::NormalText).color());

    setLinkColor(scheme.foreground(KColorScheme::LinkText).color());
    setVisitedLinkColor(scheme.foreground(KColorScheme::VisitedText).color());

    //legacy stuff
    const KColorScheme buttonScheme(QPalette::Active, KColorScheme::ColorSet::Button);
    m_buttonTextColor = buttonScheme.foreground(KColorScheme::NormalText).color();
    m_buttonBackgroundColor = buttonScheme.background(KColorScheme::NormalBackground).color();
    m_buttonHoverColor = buttonScheme.decoration(KColorScheme::HoverColor).color();
    m_buttonFocusColor = buttonScheme.decoration(KColorScheme::FocusColor).color();

    const KColorScheme viewScheme(QPalette::Active, KColorScheme::ColorSet::View);
    m_viewTextColor = viewScheme.foreground(KColorScheme::NormalText).color();
    m_viewBackgroundColor = viewScheme.background(KColorScheme::NormalBackground).color();
    m_viewHoverColor = viewScheme.decoration(KColorScheme::HoverColor).color();
    m_viewFocusColor = viewScheme.decoration(KColorScheme::FocusColor).color();

    emit colorsChanged();
}

QColor PlasmaDesktopTheme::buttonTextColor() const
{
    qWarning()<<"WARNING: buttonTextColor is deprecated, use textColor with colorSet: Theme.Button instead";
    return m_buttonTextColor;
}

QColor PlasmaDesktopTheme::buttonBackgroundColor() const
{
    qWarning()<<"WARNING: buttonBackgroundColor is deprecated, use backgroundColor with colorSet: Theme.Button instead";
    return m_buttonBackgroundColor;
}

QColor PlasmaDesktopTheme::buttonHoverColor() const
{
    qWarning()<<"WARNING: buttonHoverColor is deprecated, use backgroundColor with colorSet: Theme.Button instead";
    return m_buttonHoverColor;
}

QColor PlasmaDesktopTheme::buttonFocusColor() const
{
    qWarning()<<"WARNING: buttonFocusColor is deprecated, use backgroundColor with colorSet: Theme.Button instead";
    return m_buttonFocusColor;
}


QColor PlasmaDesktopTheme::viewTextColor() const
{
    qWarning()<<"WARNING: viewTextColor is deprecated, use backgroundColor with colorSet: Theme.View instead";
    return m_viewTextColor;
}

QColor PlasmaDesktopTheme::viewBackgroundColor() const
{
    qWarning()<<"WARNING: viewBackgroundColor is deprecated, use backgroundColor with colorSet: Theme.View instead";
    return m_viewBackgroundColor;
}

QColor PlasmaDesktopTheme::viewHoverColor() const
{
    qWarning()<<"WARNING: viewHoverColor is deprecated, use backgroundColor with colorSet: Theme.View instead";
    return m_viewHoverColor;
}

QColor PlasmaDesktopTheme::viewFocusColor() const
{
    qWarning()<<"WARNING: viewFocusColor is deprecated, use backgroundColor with colorSet: Theme.View instead";
    return m_viewFocusColor;
}

#include "moc_plasmadesktoptheme.cpp"
