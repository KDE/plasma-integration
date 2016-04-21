/*  This file is part of the KDE libraries
 *  Copyright 2016 Martin Gräßlin <mgraesslin@kde.org>
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
#include "../src/platformtheme/khintssettings.h"
#include <config-platformtheme.h>
#include <QTest>
#include <QDialogButtonBox>
#include <KSharedConfig>

class KHintsSettingsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDefaults();
};

void KHintsSettingsTest::testDefaults()
{
    // this test verifies that default are correctly loaded if there is no config yet
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    KHintsSettings hints(config);

    QCOMPARE(hints.hint(QPlatformTheme::CursorFlashTime).toInt(), 1000);
    QCOMPARE(hints.hint(QPlatformTheme::MouseDoubleClickInterval).toInt(), 400);
    QCOMPARE(hints.hint(QPlatformTheme::StartDragDistance).toInt(), 10);
    QCOMPARE(hints.hint(QPlatformTheme::StartDragTime).toInt(), 500);

    QCOMPARE(hints.hint(QPlatformTheme::ToolButtonStyle).toInt(), int(Qt::ToolButtonTextBesideIcon));

    QCOMPARE(hints.hint(QPlatformTheme::ToolBarIconSize).toInt(), 22);
    QCOMPARE(hints.hint(QPlatformTheme::ItemViewActivateItemOnSingleClick).toBool(), true);

    QCOMPARE(hints.hint(QPlatformTheme::SystemIconThemeName).toString(), QStringLiteral("breeze"));
    QCOMPARE(hints.hint(QPlatformTheme::SystemIconFallbackThemeName).toString(), QStringLiteral("hicolor"));
    QCOMPARE(hints.hint(QPlatformTheme::IconThemeSearchPaths).toStringList(), hints.xdgIconThemePaths());

    const QStringList expectedStyles = QStringList{QStringLiteral(BREEZE_STYLE_NAME), QStringLiteral("oxygen"), QStringLiteral("fusion"), QStringLiteral("windows")};
    QEXPECT_FAIL("", "Empty entry included", Continue);
    QCOMPARE(hints.hint(QPlatformTheme::StyleNames).toStringList(), expectedStyles);

    QCOMPARE(hints.hint(QPlatformTheme::DialogButtonBoxLayout).toInt(), int(QDialogButtonBox::KdeLayout));
    QCOMPARE(hints.hint(QPlatformTheme::DialogButtonBoxButtonsHaveIcons).toBool(), true);
    QCOMPARE(hints.hint(QPlatformTheme::UseFullScreenForPopupMenu).toBool(), true);
    QCOMPARE(hints.hint(QPlatformTheme::KeyboardScheme).toInt(), int(QPlatformTheme::KdeKeyboardScheme));
    QCOMPARE(hints.hint(QPlatformTheme::UiEffects).toInt(), 0);
    QCOMPARE(hints.hint(QPlatformTheme::IconPixmapSizes).value<QList<int>>(), QList<int>({512, 256, 128, 64, 32, 22, 16, 8}));
    QCOMPARE(hints.hint(QPlatformTheme::WheelScrollLines).toInt(), 3);
}

QTEST_GUILESS_MAIN(KHintsSettingsTest)

#include "khintssettings_unittest.moc"
