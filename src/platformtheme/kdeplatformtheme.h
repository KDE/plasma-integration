/*  This file is part of the KDE libraries
 *  Copyright 2013 Kevin Ottens <ervin+bluesystems@kde.org>
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

#ifndef KDEPLATFORMTHEME_H
#define KDEPLATFORMTHEME_H

#include <qpa/qplatformtheme.h>

#include <QHash>
#include <QObject>
#include <QKeySequence>

class KHintsSettings;
class KFontSettingsData;
class KWaylandIntegration;
class X11Integration;
class QIconEngine;
class QWindow;

class AltKeyEventListener;

class KdePlatformTheme : public QPlatformTheme
{
public:
    KdePlatformTheme();
    ~KdePlatformTheme();

    QVariant themeHint(ThemeHint hint) const Q_DECL_OVERRIDE;
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    QIcon fileIcon(const QFileInfo &fileInfo,
                           QPlatformTheme::IconOptions iconOptions) const override;
#else
    QPixmap fileIconPixmap(const QFileInfo &fileInfo, const QSizeF &size,
                                   QPlatformTheme::IconOptions iconOptions) const override;
    // this will be the implementation
    QIcon fileIcon(const QFileInfo &fileInfo,
                           QPlatformTheme::IconOptions iconOptions) const;
#endif
    const QPalette *palette(Palette type = SystemPalette) const Q_DECL_OVERRIDE;
    const QFont *font(Font type) const Q_DECL_OVERRIDE;
    QIconEngine *createIconEngine(const QString &iconName) const Q_DECL_OVERRIDE;
    QList<QKeySequence> keyBindings(QKeySequence::StandardKey key) const Q_DECL_OVERRIDE;

    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const Q_DECL_OVERRIDE;
    bool usePlatformNativeDialog(DialogType type) const Q_DECL_OVERRIDE;

    QString standardButtonText(int button) const Q_DECL_OVERRIDE;

    QPlatformSystemTrayIcon *createPlatformSystemTrayIcon() const Q_DECL_OVERRIDE;

#if QT_VERSION >= QT_VERSION_CHECK(5,7,0)
    QPlatformMenuBar *createPlatformMenuBar() const Q_DECL_OVERRIDE;
#endif

private:
    void loadSettings();

    static void setWindowProperty(QWindow *window, const QByteArray &name, const QByteArray &value);

    KHintsSettings *m_hints;
    KFontSettingsData *m_fontsData;
    QScopedPointer<KWaylandIntegration> m_kwaylandIntegration;
    QScopedPointer<X11Integration> m_x11Integration;

};

#endif // KDEPLATFORMTHEME_H
