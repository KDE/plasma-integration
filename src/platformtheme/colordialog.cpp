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
#include <QPushButton>
#include "colordialog.h"

ColorDialog::ColorDialog() : QDialog()
{
    setLayout(new QStackedLayout);

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
}

bool ColorDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parentWindow)
{
    if (dialog.isNull()) {
        dialog = new ColorDialog;
        dialog->setMinimumSize(QSize(500, 400));
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

}

QColor ColorDialogHelper::currentColor() const
{
    return QColor();
}