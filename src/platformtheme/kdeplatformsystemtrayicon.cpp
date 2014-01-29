/*  This file is part of the KDE libraries
 *  Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
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
#include "kdeplatformsystemtrayicon.h"
#include <kstatusnotifieritem.h>
#include <QIcon>
#include <QRect>
#include <QApplication>
#include <qpa/qplatformmenu.h>

KDEPlatformSystemTrayIcon::KDEPlatformSystemTrayIcon()
    : QPlatformSystemTrayIcon()
    , m_sni(Q_NULLPTR)
{
}

KDEPlatformSystemTrayIcon::~KDEPlatformSystemTrayIcon()
{
}

void KDEPlatformSystemTrayIcon::init()
{
    if (!m_sni) {
        m_sni = new KStatusNotifierItem();
        m_sni->setTitle(QApplication::applicationDisplayName());
        connect(m_sni, &KStatusNotifierItem::activateRequested, [this](bool active, const QPoint &pos) {
            Q_UNUSED(active)
            Q_UNUSED(pos)
            emit activated(QPlatformSystemTrayIcon::Trigger);
        });
        connect(m_sni, &KStatusNotifierItem::secondaryActivateRequested, [this](const QPoint &pos) {
            Q_UNUSED(pos)
            emit activated(QPlatformSystemTrayIcon::Context);
        });
    }
}

void KDEPlatformSystemTrayIcon::cleanup()
{
    delete m_sni;
    m_sni = Q_NULLPTR;
}

void KDEPlatformSystemTrayIcon::updateIcon(const QIcon &icon)
{
    if (!m_sni) {
        return;
    }
    if (icon.name().isEmpty()) {
        m_sni->setIconByPixmap(icon);
        m_sni->setToolTipIconByPixmap(icon);
    } else {
        m_sni->setIconByName(icon.name());
        m_sni->setToolTipIconByName(icon.name());
    }
}

void KDEPlatformSystemTrayIcon::updateToolTip(const QString &tooltip)
{
    if (!m_sni) {
        return;
    }
    m_sni->setToolTipTitle(tooltip);
}

void KDEPlatformSystemTrayIcon::updateMenu(QPlatformMenu *menu)
{
    // ok this really sucks. We don't want create our menus in the platform theme
    // such the menu is null.
    Q_UNUSED(menu)
}

QRect KDEPlatformSystemTrayIcon::geometry() const
{
    // StatusNotifierItem doesn't provide the geometry
    return QRect();
}

void KDEPlatformSystemTrayIcon::showMessage(const QString &msg, const QString &title,
                                            const QIcon &icon, MessageIcon iconType, int secs)
{
    Q_UNUSED(iconType)
    if (!m_sni) {
        return;
    }
    m_sni->showMessage(title, msg, icon.themeName(), secs);
}

bool KDEPlatformSystemTrayIcon::isSystemTrayAvailable() const
{
    // TODO: check on DBus
    return true;
}

bool KDEPlatformSystemTrayIcon::supportsMessages() const
{
    return true;
}
