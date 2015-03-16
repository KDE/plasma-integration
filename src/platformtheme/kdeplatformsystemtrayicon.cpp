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
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QRect>
#include <QApplication>
#include <QDBusInterface>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
SystemTrayMenu::SystemTrayMenu()
    : QPlatformMenu()
    , m_tag(0)
    , m_menu(new QMenu())
{
    connect(m_menu.data(), &QMenu::aboutToShow, this, &QPlatformMenu::aboutToShow);
    connect(m_menu.data(), &QMenu::aboutToHide, this, &QPlatformMenu::aboutToHide);
}

SystemTrayMenu::~SystemTrayMenu()
{
    if (m_menu) {
        m_menu->deleteLater();
    }
}

QPlatformMenuItem *SystemTrayMenu::createMenuItem() const
{
    return new SystemTrayMenuItem();
}

void SystemTrayMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem)) {
        bool inserted = false;
        if (SystemTrayMenuItem *oursBefore = qobject_cast<SystemTrayMenuItem*>(before)) {
            for (auto it = m_items.begin(); it != m_items.end(); ++it) {
                if (*it == oursBefore) {
                    m_items.insert(it, ours);
                    if (m_menu) {
                        m_menu->insertAction(oursBefore->action(), ours->action());
                    }
                    inserted = true;
                    break;
                }
            }
        }
        if (!inserted) {
            m_items.append(ours);
            if (m_menu) {
                m_menu->addAction(ours->action());
            }
        }
    }
}

QPlatformMenuItem *SystemTrayMenu::menuItemAt(int position) const
{
    if (position < m_items.size()) {
        return m_items.at(position);
    }
    return Q_NULLPTR;
}

QPlatformMenuItem *SystemTrayMenu::menuItemForTag(quintptr tag) const
{
    auto it = std::find_if(m_items.constBegin(), m_items.constEnd(), [tag](SystemTrayMenuItem *item) {
        return item->tag() == tag;
    });
    if (it != m_items.constEnd()) {
        return *it;
    }
    return Q_NULLPTR;
}

void SystemTrayMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem)) {
        m_items.removeOne(ours);
        if (ours->action() && m_menu) {
            m_menu->removeAction(ours->action());
        }
    }
}

void SystemTrayMenu::setEnabled(bool enabled)
{
    if (!m_menu) {
        return;
    }
    m_menu->setEnabled(enabled);
}

void SystemTrayMenu::setIcon(const QIcon &icon)
{
    if (!m_menu) {
        return;
    }
    m_menu->setIcon(icon);
}

void SystemTrayMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenu::setText(const QString &text)
{
    if (!m_menu) {
        return;
    }
    m_menu->setTitle(text);
}

void SystemTrayMenu::setVisible(bool visible)
{
    if (!m_menu) {
        return;
    }
    m_menu->setVisible(visible);
}

void SystemTrayMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem)
    // nothing to do
}

void SystemTrayMenu::syncSeparatorsCollapsible(bool enable)
{
    if (!m_menu) {
        return;
    }
    m_menu->setSeparatorsCollapsible(enable);
}

quintptr SystemTrayMenu::tag() const
{
    return m_tag;
}

QMenu *SystemTrayMenu::menu() const
{
    return m_menu.data();
}

SystemTrayMenuItem::SystemTrayMenuItem()
    : QPlatformMenuItem()
    , m_tag(0)
    , m_action(new QAction(this))
{
    connect(m_action, &QAction::triggered, this, &QPlatformMenuItem::activated);
    connect(m_action, &QAction::hovered, this, &QPlatformMenuItem::hovered);
}

SystemTrayMenuItem::~SystemTrayMenuItem()
{
}

void SystemTrayMenuItem::setCheckable(bool checkable)
{
    m_action->setCheckable(checkable);
}

void SystemTrayMenuItem::setChecked(bool isChecked)
{
    m_action->setChecked(isChecked);
}

void SystemTrayMenuItem::setEnabled(bool enabled)
{
    m_action->setEnabled(enabled);
}

void SystemTrayMenuItem::setFont(const QFont &font)
{
    m_action->setFont(font);
}

void SystemTrayMenuItem::setIcon(const QIcon &icon)
{
    m_action->setIcon(icon);
}

void SystemTrayMenuItem::setIsSeparator(bool isSeparator)
{
    m_action->setSeparator(isSeparator);
}

void SystemTrayMenuItem::setMenu(QPlatformMenu *menu)
{
    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu *>(menu)) {
        m_action->setMenu(ourMenu->menu());
    }
}

void SystemTrayMenuItem::setRole(QPlatformMenuItem::MenuRole role)
{
    Q_UNUSED(role)
}

void SystemTrayMenuItem::setShortcut(const QKeySequence &shortcut)
{
    m_action->setShortcut(shortcut);
}

void SystemTrayMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenuItem::setText(const QString &text)
{
    m_action->setText(text);
}

void SystemTrayMenuItem::setVisible(bool isVisible)
{
    m_action->setVisible(isVisible);
}

void SystemTrayMenuItem::setIconSize(int size)
{
    Q_UNUSED(size);
}

quintptr SystemTrayMenuItem::tag() const
{
    return m_tag;
}

QAction *SystemTrayMenuItem::action() const
{
    return m_action;
}
#endif

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if (!m_sni) {
        return;
    }
    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu*>(menu)) {
        m_sni->setContextMenu(ourMenu->menu());
    }
#else
    Q_UNUSED(menu)
#endif
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
QPlatformMenu *KDEPlatformSystemTrayIcon::createMenu() const
{
    return new SystemTrayMenu();
}
#endif

QRect KDEPlatformSystemTrayIcon::geometry() const
{
    // StatusNotifierItem doesn't provide the geometry
    return QRect();
}

void KDEPlatformSystemTrayIcon::showMessage(const QString &title, const QString &msg,
                                            const QIcon &icon, MessageIcon iconType, int secs)
{
    Q_UNUSED(iconType)
    if (!m_sni) {
        return;
    }
    m_sni->showMessage(title, msg, icon.name(), secs);
}

bool KDEPlatformSystemTrayIcon::isSystemTrayAvailable() const
{
    QDBusInterface systrayHost("org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher");
    if (systrayHost.isValid()) {
        return systrayHost.property("IsStatusNotifierHostRegistered").toBool();
    }

    return false;
}

bool KDEPlatformSystemTrayIcon::supportsMessages() const
{
    return true;
}
