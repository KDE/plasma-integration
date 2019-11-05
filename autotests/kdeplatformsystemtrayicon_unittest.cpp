/*  This file is part of the KDE libraries
 *  Copyright 2019 Konrad Materka <materka@gmail.com>
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

#include <QMenu>
#include <QSignalSpy>
#include <QTest>

#include "../src/platformtheme/kdeplatformsystemtrayicon.h"

class KDEPlatformSystemTrayIcon_UnitTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // test for BUG 365105
    void testMenuRecreate();
    void testAddActionAfterMenuRecreate();
};

void KDEPlatformSystemTrayIcon_UnitTest::testMenuRecreate()
{
    QMenu *trayIconMenu = new QMenu();
    trayIconMenu->addAction("testAction");

    KDEPlatformSystemTrayIcon *kpsti = new KDEPlatformSystemTrayIcon();

    // simulates first QSystemTrayIcon::show()
    kpsti->init();
    SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu*>(kpsti->createMenu());
    trayIconMenu->setPlatformMenu(ourMenu);
    kpsti->updateMenu(trayIconMenu->platformMenu());

    QMenu *firstMenu = ourMenu->menu();
    QSignalSpy menuDestroyedSpy(firstMenu, SIGNAL(destroyed()));
    QCOMPARE(firstMenu->actions().size(), 1);
    QCOMPARE(firstMenu->actions().first()->text(), "testAction");

    // simulates QSystemTrayIcon::hide()
    kpsti->cleanup();

    // simulates second QSystemTrayIcon::show()
    kpsti->init();
    kpsti->updateMenu(trayIconMenu->platformMenu());

    QMenu *recreatedMenu = ourMenu->menu();
    QVERIFY(firstMenu != recreatedMenu);
    QCOMPARE(recreatedMenu->actions().size(), 1);
    QCOMPARE(recreatedMenu->actions().first()->text(), "testAction");
    QCOMPARE(menuDestroyedSpy.count(), 1);
}

void KDEPlatformSystemTrayIcon_UnitTest::testAddActionAfterMenuRecreate()
{
    QMenu *trayIconMenu = new QMenu();
    trayIconMenu->addAction("testAction1");

    KDEPlatformSystemTrayIcon *kpsti = new KDEPlatformSystemTrayIcon();

    // simulates first QSystemTrayIcon::show()
    kpsti->init();
    SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu*>(kpsti->createMenu());
    trayIconMenu->setPlatformMenu(ourMenu);
    kpsti->updateMenu(trayIconMenu->platformMenu());

    QMenu *firstMenu = ourMenu->menu();
    QSignalSpy menuDestroyedSpy(firstMenu, SIGNAL(destroyed()));
    QCOMPARE(firstMenu->actions().size(), 1);
    QCOMPARE(firstMenu->actions().first()->text(), "testAction1");

    // simulates QSystemTrayIcon::hide()
    kpsti->cleanup();

    // add action, internal menu is destroyed
    trayIconMenu->addAction("testAction2");

    // simulates second QSystemTrayIcon::show()
    kpsti->init();
    kpsti->updateMenu(trayIconMenu->platformMenu());

    QMenu *recreatedMenu = ourMenu->menu();
    QVERIFY(firstMenu != recreatedMenu);
    QCOMPARE(recreatedMenu->actions().size(), 2);
    QCOMPARE(recreatedMenu->actions().first()->text(), "testAction1");
    QCOMPARE(recreatedMenu->actions().last()->text(), "testAction2");
    QCOMPARE(menuDestroyedSpy.count(), 1);
}

QTEST_MAIN(KDEPlatformSystemTrayIcon_UnitTest)

#include "kdeplatformsystemtrayicon_unittest.moc"
