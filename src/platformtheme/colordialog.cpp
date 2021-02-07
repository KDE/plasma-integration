/*  This file is part of the KDE libraries
 *  Copyright 2020 Carson Black <uhhadd@gmail.com>
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

#include <QQmlApplicationEngine>
#include <QStackedLayout>
#include <QPainter>
#include <QPushButton>
#include <QQuickItem>
#include <QtMath>
#include "colordialog.h"

ColorDialog::ColorDialog() : QDialog()
{
    setLayout(new QStackedLayout);

    qmlRegisterType<HSVCircle>("org.kde.private.plasmaintegration", 1, 0, "HSVCircle");

    view = new QQuickWidget(this);
    const QUrl dialogURL(QStringLiteral("qrc:/org/kde/plasma/integration/ColorDialog.qml"));
    QObject::connect(
        view, &QQuickWidget::statusChanged,
        [=](QQuickWidget::Status status) {
            if (status == QQuickWidget::Error) {
                qDebug() << view->errors();
                qFatal("Failed to load color dialog.");
            }
        }
    );
    view->setSource(dialogURL);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    layout()->addWidget(view);
    layout()->setContentsMargins(0, 0, 0, 0);

    setMinimumSize(400, 500);
    setBaseSize(400, 500);
}

ColorDialogHelper::ColorDialogHelper() : QPlatformColorDialogHelper()
{

}

ColorDialogHelper::~ColorDialogHelper()
{

}

void ColorDialogHelper::exec()
{
    dialog->exec();
    Q_EMIT colorSelected(currentColor());
    Q_EMIT accept();
}

void ColorDialogHelper::prepareDialog()
{
    dialog = new ColorDialog;
    dialog->setMinimumSize(QSize(500, 400));

    QObject::connect(dialog, &QDialog::finished, this, [=] {
        Q_EMIT colorSelected(currentColor());
    });

    connect(dialog->view.data(), SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
}

bool ColorDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parentWindow)
{
    if (dialog.isNull()) {
        prepareDialog();
    }

    dialog->setWindowModality(modality);
    dialog->setWindowFlags(windowFlags);

    dialog->winId();
    dialog->windowHandle()->setTransientParent(parentWindow);

    dialog->show();
    return true;
}

void ColorDialogHelper::hide()
{
    dialog->hide();
}

void ColorDialogHelper::setCurrentColor(const QColor& color)
{
    if (dialog.isNull()) {
        return;
    }

    dialog->view->rootObject()->setProperty("currentColor", color);
}

QVariant ColorDialogHelper::styleHint(StyleHint hint) const
{
    if (hint == DialogIsQtWindow) {
        return true;
    }

    return QPlatformDialogHelper::styleHint(hint);
}

QColor ColorDialogHelper::currentColor() const
{
    return dialog->view->rootObject()->property("currentColor").value<QColor>();
}

QColor HSVCircle::mapToRGB(int x, int y) const
{
    Q_ASSERT(width() == height());
    const auto size = int(width());
    const auto radius = size / 2;

    const auto h = (qAtan2(x - radius, y - radius) + M_PI) / (2.0 * M_PI);
    const auto s = qSqrt(qPow(x - radius, 2) + qPow(y - radius, 2)) / radius;
    const auto v = value;

    return QColor::fromHsvF(qBound(0.0, h, 1.0), qBound(0.0, s, 1.0), qBound(0.0, v, 1.0));
}

HSVCircle::HSVCircle(QQuickItem* parent) : QQuickPaintedItem(parent)
{
    connect(this, &HSVCircle::valueChanged, [this] {
        update();
    });
}

void HSVCircle::paint(QPainter *painter)
{
    Q_ASSERT(width() == height());
    const auto size = int(width());

    QRgb* pixels = new QRgb[size*size];

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            pixels[x+y*size] = mapToRGB(x, y).rgb();
        }
    }

    painter->drawImage(0, 0, QImage((uchar*)pixels, width(), height(), QImage::Format_ARGB32));
    delete[] pixels;
}
