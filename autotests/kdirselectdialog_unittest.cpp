/*  This file is part of the KDE libraries
 *  Copyright 2015 Alejandro Fiestas Olivares <afiestas@kde.org>
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

#include <QTest>
#include "../src/platformtheme/kfiletreeview_p.h"
#include "../src/platformtheme/kdirselectdialog_p.h"

class KDirSelectDialog_UnitTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSetCurrentUrl_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<QUrl>("expectedUrl");

        QTest::newRow("only_scheme") << QUrl("smb:") << QUrl("smb:/");
        QTest::newRow("with_no_host") << QUrl("smb://") << QUrl("smb://");
        QTest::newRow("with_root_path") << QUrl("smb:///") << QUrl("smb:///");
    }

    void testSetCurrentUrl()
    {
        QFETCH(QUrl, url);
        QFETCH(QUrl, expectedUrl);

        KDirSelectDialog dirDialog;
        dirDialog.setCurrentUrl(url);

        QCOMPARE(dirDialog.rootUrl(), expectedUrl);
    }
};

QTEST_MAIN(KDirSelectDialog_UnitTest)

#include "kdirselectdialog_unittest.moc"

