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
#ifndef KDEPLATFORMSYSTEMTRAYICON_H
#define KDEPLATFORMSYSTEMTRAYICON_H

#include <qpa/qplatformsystemtrayicon.h>

class KStatusNotifierItem;

class KDEPlatformSystemTrayIcon : public QPlatformSystemTrayIcon
{
public:
    KDEPlatformSystemTrayIcon();
    ~KDEPlatformSystemTrayIcon() Q_DECL_OVERRIDE;

    void init() Q_DECL_OVERRIDE;
    void cleanup() Q_DECL_OVERRIDE;
    void updateIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void updateToolTip(const QString &tooltip) Q_DECL_OVERRIDE;
    void updateMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    QRect geometry() const Q_DECL_OVERRIDE;
    void showMessage(const QString &msg, const QString &title,
                     const QIcon &icon, MessageIcon iconType, int secs) Q_DECL_OVERRIDE;

    bool isSystemTrayAvailable() const Q_DECL_OVERRIDE;
    bool supportsMessages() const Q_DECL_OVERRIDE;

private:
    KStatusNotifierItem *m_sni;
};

#endif
