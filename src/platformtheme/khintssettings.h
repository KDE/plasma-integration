/*  This file is part of the KDE libraries
 *  Copyright 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
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

#ifndef KHINTS_SETTINGS_H
#define KHINTS_SETTINGS_H

#include <QDBusVariant>
#include <QObject>
#include <QVariant>

#include <qpa/qplatformtheme.h>
#include <ksharedconfig.h>

class KConfigGroup;

class QPalette;
class KHintsSettings : public QObject
{
    Q_OBJECT
public:
    /**
    * An identifier for change signals.
    * @note Copied from KGlobalSettings
    */
    enum ChangeType { PaletteChanged = 0, FontChanged, StyleChanged,
                      SettingsChanged, IconChanged, CursorChanged,
                      ToolbarStyleChanged, ClipboardConfigChanged,
                      BlockShortcuts, NaturalSortingChanged
                    };
    /**
    * Valid values for the settingsChanged signal
    * @note Copied from KGlobalSettings
    */
    enum SettingsCategory { SETTINGS_MOUSE, SETTINGS_COMPLETION, SETTINGS_PATHS,
                            SETTINGS_POPUPMENU, SETTINGS_QT, SETTINGS_SHORTCUTS,
                            SETTINGS_LOCALE, SETTINGS_STYLE
                          };
    explicit KHintsSettings(KSharedConfig::Ptr kdeglobals = KSharedConfig::Ptr());
    ~KHintsSettings() override;

    inline QVariant hint(QPlatformTheme::ThemeHint hint) const
    {
        return m_hints[hint];
    }

    inline QPalette *palette(QPlatformTheme::Palette type) const
    {
        return m_palettes[type];
    }
    QStringList xdgIconThemePaths() const;

private Q_SLOTS:
    void delayedDBusConnects();
    void setupIconLoader();
    void toolbarStyleChanged();
    void slotNotifyChange(int type, int arg);
    void slotPortalSettingChanged(const QString &group, const QString &key, const QDBusVariant &value);

private:
    QVariant readConfigValue(const QString &group, const QString &key, const QVariant &defaultValue);
    QVariant readConfigValue(const KConfigGroup &cg, const QString &key, const QVariant &defaultValue) const;
    void loadPalettes();
    void iconChanged(int group);
    void updateQtSettings(KConfigGroup &cg);
    void updateShowIconsInMenuItems(KConfigGroup &cg);
    Qt::ToolButtonStyle toolButtonStyle(const KConfigGroup &cg);
    void updateCursorTheme();
    void updatePortalSetting();

    QHash<QPlatformTheme::Palette, QPalette *> m_palettes;
    QHash<QPlatformTheme::ThemeHint, QVariant> m_hints;
    KSharedConfigPtr mKdeGlobals;
    KSharedConfigPtr mDefaultLnfConfig;
    KSharedConfigPtr mLnfConfig;
    QMap<QString, QVariantMap> mKdeGlobalsPortal;
    bool mUsePortal;
};

#endif //KHINTS_SETTINGS_H
