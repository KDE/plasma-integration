// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>

#include "messageboxsoundhandlerqt.h"

#include <QApplication>
#include <QEvent>
#include <QMessageBox>

#include <KNotification>

MessageBoxSoundHandlerQt::MessageBoxSoundHandlerQt(QObject *parent)
    : QObject(parent)
{
    qApp->installEventFilter(this);
}

bool MessageBoxSoundHandlerQt::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Show && !event->spontaneous()) {
        if (auto messageBox = qobject_cast<QMessageBox *>(watched)) {
            QString messageType;
            switch (messageBox->icon()) {
            case QMessageBox::NoIcon:
            case QMessageBox::Information:
                messageType = QStringLiteral("messageInformation");
                break;
            case QMessageBox::Warning:
                messageType = QStringLiteral("messageWarning");
                break;
            case QMessageBox::Critical:
                messageType = QStringLiteral("messageCritical");
                break;
            case QMessageBox::Question:
                messageType = QStringLiteral("messageQuestion");
                break;
            }

            KNotification::event(messageType,
                                 messageBox->windowTitle(),
                                 messageBox->text(),
                                 QPixmap(),
                                 KNotification::DefaultEvent | KNotification::CloseOnTimeout);
        }
    }

    return QObject::eventFilter(watched, event);
}
