/*  This file is part of the KDE libraries
 *  Copyright 2014 Dominik Haumann <dhaumann@kde.org>
 *  Copyright 2015 David Rosca <nowrep@gmail.com>
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
#include <QQmlEngine>
#include <QQmlComponent>
#include <KFileWidget>

class KFileDialogQml_UnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testShowDialogParentless()
    {
        KFileWidget *fw;
        {
            QQmlEngine engine;
            QQmlComponent component(&engine);
            component.loadUrl(QUrl::fromLocalFile(QFINDTESTDATA("qml/filedialog_parentless.qml")));
            component.create();

            fw = findFileWidget();
            QVERIFY(fw);
            // real show() is delayed to next event.
            QTest::qWaitForWindowExposed(fw->window());
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
        delete fw;
    }

    void testShowDialogWithParent()
    {
        KFileWidget *fw;
        {
            QQmlEngine engine;
            QQmlComponent component(&engine);
            component.loadUrl(QUrl::fromLocalFile(QFINDTESTDATA("qml/filedialog_withparent.qml")));
            component.create();

            fw = findFileWidget();
            QVERIFY(fw);
            // real show() is delayed to next event.
            QTest::qWaitForWindowExposed(fw->window());
            QCOMPARE(fw->isVisible(), true);
            fw->slotCancel();
        }
        delete fw;
    }

private:
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

QTEST_MAIN(KFileDialogQml_UnitTest)

#include "kfiledialogqml_unittest.moc"

