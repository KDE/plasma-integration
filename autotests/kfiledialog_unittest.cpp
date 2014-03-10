/*  This file is part of the KDE libraries
 *  Copyright 2014 Dominik Haumann <dhaumann@kde.org>
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
#include <QFileDialog>

#include <QDebug>

class KFileDialog_UnitTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testSetNameFilters()
    {
        QFileDialog dialog;

        QStringList nameFilterList = QStringList() << "c (*.cpp)" << "h (*.h)";
        dialog.setNameFilters(nameFilterList);
        QCOMPARE(dialog.nameFilters(), nameFilterList);
    }

    void testSelectNameFilter()
    {
        QFileDialog dialog;

        QStringList nameFilterList = QStringList() << "c (*.cpp)" << "h (*.h)";
        dialog.setNameFilters(nameFilterList);
        QCOMPARE(dialog.nameFilters(), nameFilterList);

        QString selectNameFilter("h (*.h)");
        dialog.selectNameFilter(selectNameFilter);
        QEXPECT_FAIL("", "Does currently not work. Works, once the dialog gets shown, though.", Continue);
        QCOMPARE(dialog.selectedNameFilter(), selectNameFilter);

        dialog.show();
        QCOMPARE(dialog.selectedNameFilter(), selectNameFilter);
    }

};

QTEST_MAIN(KFileDialog_UnitTest)

#include "kfiledialog_unittest.moc"

