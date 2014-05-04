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
#include <KFileWidget>
#include <KDirOperator>

Q_DECLARE_METATYPE(QFileDialog::ViewMode)
Q_DECLARE_METATYPE(QFileDialog::FileMode)
Q_DECLARE_METATYPE(KFile::FileView)
Q_DECLARE_METATYPE(KFile::Modes)

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

    void testSetDirectory()
    {
        QFileDialog dialog;
        dialog.setDirectory(QDir::rootPath());
        QCOMPARE(dialog.directory().absolutePath(), QDir::rootPath());
    }

    void testSetView_data()
    {
        QTest::addColumn<QFileDialog::ViewMode>("qtViewMode");
        QTest::addColumn<KFile::FileView>("kdeViewMode");
        QTest::newRow("detail") << QFileDialog::Detail << KFile::Detail;
        QTest::newRow("list") << QFileDialog::List << KFile::Simple;
    }

    void testSetView()
    {
        QFETCH(QFileDialog::ViewMode, qtViewMode);
        QFETCH(KFile::FileView, kdeViewMode);
        QFileDialog dialog;
        dialog.setViewMode(qtViewMode);
        dialog.show();

        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            KFileWidget * fw = widget->findChild<KFileWidget *>();
            if(fw) {
                QCOMPARE(fw->dirOperator()->viewMode(), kdeViewMode);
            }
        }

/*
 * Disabled because of a bug in Qt that causes the wrong viewMode value to be returned. A patch
 * is in Qt's gerrit: https://codereview.qt-project.org/#change,84137
 * It's not yet in Qt 5.4 therefore Qt 5.5 is the minimal version with this patch. However, the
 * patch is likely going to be accepted earlier. The version check below will have to
 * be changed accordingly once the patch lands.
 */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        QCOMPARE(dialog.viewMode(), qtViewMode);
#endif
    }

    void testSetFileMode_data()
    {
        QTest::addColumn<QFileDialog::FileMode>("qtFileMode");
        QTest::addColumn<KFile::Modes>("kdeFileMode");
        QTest::newRow("anyfile") << QFileDialog::AnyFile << KFile::Modes(KFile::File);
        QTest::newRow("existingfile") << QFileDialog::ExistingFile << KFile::Modes(KFile::File | KFile::ExistingOnly);
        QTest::newRow("directory") << QFileDialog::Directory << KFile::Modes(KFile::Directory);
        QTest::newRow("existingfiles") << QFileDialog::ExistingFiles << KFile::Modes(KFile::Files | KFile::ExistingOnly);
    }

    void testSetFileMode()
    {
        QFETCH(QFileDialog::FileMode, qtFileMode);
        QFETCH(KFile::Modes, kdeFileMode);
        QFileDialog dialog;
        dialog.setFileMode(qtFileMode);
        dialog.show();

        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            KFileWidget * fw = widget->findChild<KFileWidget *>();
            if(fw) {
                QCOMPARE(fw->mode(), kdeFileMode);
            }
        }

        QCOMPARE(dialog.fileMode(), qtFileMode);
    }
};

QTEST_MAIN(KFileDialog_UnitTest)

#include "kfiledialog_unittest.moc"

