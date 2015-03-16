/*  This file is part of the KDE libraries
 *  Copyright 2015 Martin Gräßlin <mgraesslin@kde.org>
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

#include <QAbstractEventDispatcher>
#include <QMenu>
#include <QSignalSpy>
#include <QSystemTrayIcon>
#include <QTest>

class KSniUnitTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // test for BUG 343976
    void testHideDontCrash();
};

void KSniUnitTest::testHideDontCrash()
{
    QSystemTrayIcon *sti = new QSystemTrayIcon(this);
    QMenu *trayIconMenu = new QMenu();
    QAction *dummyAction = new QAction(QStringLiteral("foo"), sti);
    trayIconMenu->addAction(dummyAction);
    QSignalSpy menuDestroyedSpy(trayIconMenu, SIGNAL(destroyed()));
    QVERIFY(menuDestroyedSpy.isValid());
    sti->setContextMenu(trayIconMenu);

    connect(sti, &QSystemTrayIcon::destroyed, trayIconMenu, &QMenu::deleteLater);
    sti->setVisible(true);
    sti->show();

    // now delete it
    delete sti;
    QVERIFY(menuDestroyedSpy.wait());
}

QTEST_MAIN(KSniUnitTest)

#include "ksni_unittest.moc"
