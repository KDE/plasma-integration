/* This file is part of the KDE libraries

    Copyright 2017 David Edmundson <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * This class is for checking that wayland server side window decorations remain
 * after a window is hidden and shown.
 */

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <KColorSchemeManager>
#include <QComboBox>


class ATestWindow: public QWidget
{
    Q_OBJECT
public:
    ATestWindow();

private:
    QPushButton *mBtn;
    QWidget *m_area;
};

ATestWindow::ATestWindow()
{
    mBtn = new QPushButton(QStringLiteral("Hide and Show"));

    m_area = new QWidget;
    m_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(mBtn, &QPushButton::clicked, this, [this]() {
        this->hide();
        QTimer::singleShot(1000, this, [this](){this->show();});
    });

    QComboBox *colorCombo = new QComboBox();
    KColorSchemeManager *schemes = new KColorSchemeManager(this);
    colorCombo->setModel(schemes->model());

    connect(colorCombo, QOverload<int>::of(&QComboBox::activated), schemes, [=](int row) {
        schemes->activateScheme(colorCombo->model()->index(row, 0));
    });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mBtn);
    layout->addWidget(colorCombo);
    setLayout(layout);
}



int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ATestWindow wnd;
    wnd.show();

    return app.exec();
}


#include "windowdecotest.moc"
