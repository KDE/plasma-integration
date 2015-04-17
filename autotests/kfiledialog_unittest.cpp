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

    void testViewMode()
    {
        // Open a file dialog, and change view mode to tree
        {
            QFileDialog dialog;
            dialog.show();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            fw->setViewMode(KFile::Tree);
            fw->slotCancel(); // the saving happens there
        }
        // Open another one, and check that the view mode is now tree, change it to simple
        {
            QFileDialog dialog;
            dialog.show();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            KDirOperator *op = fw->dirOperator();
            QCOMPARE(fileViewToString(op->viewMode()), fileViewToString(KFile::Tree));
            fw->setViewMode(KFile::Simple);
            fw->slotCancel();
        }
        // Open another one, and check that the view mode is now simple
        {
            QFileDialog dialog;
            dialog.show();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            KDirOperator *op = fw->dirOperator();
            QCOMPARE(fileViewToString(op->viewMode()), fileViewToString(KFile::Simple));
            fw->setViewMode(KFile::Detail);
            fw->slotCancel();
        }
    }

    void testOpenDialog()
    {
        // Open parentless
        {
            QFileDialog dialog;
            dialog.open();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
        // Open with parent
        {
            QWidget w;
            w.show();

            QFileDialog dialog(&w);
            dialog.open();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
    }

    void testShowDialog()
    {
        // Show parentless
        {
            QFileDialog dialog;
            dialog.show();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
        // Show with parent
        {
            QWidget w;
            w.show();

            QFileDialog dialog(&w);
            dialog.show();

            KFileWidget *fw = findFileWidget();
            QVERIFY(fw);
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
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

        KFileWidget *fw = findFileWidget();
        QVERIFY(fw);
        QCOMPARE(fw->mode(), kdeFileMode);

        QCOMPARE(dialog.fileMode(), qtFileMode);
    }
private:
    static QString fileViewToString(KFile::FileView fv)
    {
        switch (fv) {
            case KFile::Detail:
                return QStringLiteral("Detail");
            case KFile::Simple:
                return QStringLiteral("Simple");
            case KFile::Tree:
                return QStringLiteral("Tree");
            case KFile::DetailTree:
                return QStringLiteral("DetailTree");
            default:
                break;
        }
        return QStringLiteral("ERROR");
    }

    static KFileWidget *findFileWidget()
    {
        QList<KFileWidget *> widgets;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            KFileWidget *fw = widget->findChild<KFileWidget *>();
            if (fw) {
                widgets.append(fw);
            }
        }
        Q_ASSERT(widgets.count() == 1);
        return (widgets.count() == 1) ? widgets.first() : Q_NULLPTR;
    }
};

QTEST_MAIN(KFileDialog_UnitTest)

#include "kfiledialog_unittest.moc"

