// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "kdeplatformmessagedialoghelper.h"

#include <KNotification>
#include <KWindowSystem>

#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QWindow>

void KDEPlatformMessageDialogHelper::exec()
{
    m_box->exec();
}

bool KDEPlatformMessageDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parent)
{
    m_box = new QMessageBox();
    m_box->setWindowTitle(options()->windowTitle());
    m_box->setWindowModality(modality);
    m_box->setWindowFlags(windowFlags);
    m_box->winId();
    m_box->windowHandle()->setTransientParent(parent);
    m_box->setAttribute(Qt::WA_DeleteOnClose);
    m_box->setOption(QMessageBox::Option::DontUseNativeDialog);
    m_box->setText(options()->text());
    m_box->setDetailedText(options()->detailedText());
    m_box->setInformativeText(options()->informativeText());
    m_box->setIcon(static_cast<QMessageBox::Icon>(options()->standardIcon()));
    m_box->setIconPixmap(options()->iconPixmap());

    if (!options()->checkBoxLabel().isEmpty()) {
        auto checkBox = new QCheckBox(options()->checkBoxLabel());
        checkBox->setCheckState(options()->checkBoxState());
        m_box->setCheckBox(checkBox);
        connect(checkBox, &QCheckBox::checkStateChanged, this, &KDEPlatformMessageDialogHelper::checkBoxStateChanged);
    }

    for (const auto &button : options()->customButtons()) {
        const auto newButton = m_box->addButton(button.label, static_cast<QMessageBox::ButtonRole>(button.role));
        m_customButtonIds.insert(newButton, button.id);
    }

    for (int i = StandardButton::FirstButton; i < StandardButton::LastButton; i <<= 1) {
        if (i & options()->standardButtons()) {
            m_box->addButton(static_cast<QMessageBox::StandardButton>(i));
        }
    }

    connect(m_box, &QMessageBox::buttonClicked, this, [this](const auto &button) {
        auto standardButton = static_cast<QPlatformDialogHelper::StandardButton>(m_box->standardButton(button));
        if (standardButton == QPlatformDialogHelper::StandardButton::NoButton) {
            standardButton = static_cast<QPlatformDialogHelper::StandardButton>(m_customButtonIds[button]);
        }
        Q_EMIT clicked(standardButton, static_cast<QPlatformDialogHelper::ButtonRole>(m_box->buttonRole(button)));
    });

    // HACK: Delayed to work around QTBUG-144324
    QMetaObject::invokeMethod(
        this,
        [this]() {
            m_box->open();
        },
        Qt::QueuedConnection);

    QString messageType;
    switch (options()->standardIcon()) {
    case QMessageDialogOptions::NoIcon:
        break;
    case QMessageDialogOptions::Information:
        messageType = QStringLiteral("messageInformation");
        break;
    case QMessageDialogOptions::Warning:
        messageType = QStringLiteral("messageWarning");
        break;
    case QMessageDialogOptions::Critical:
        messageType = QStringLiteral("messageCritical");
        break;
    case QMessageDialogOptions::Question:
        messageType = QStringLiteral("messageQuestion");
        break;
    }

    if (!messageType.isEmpty()) {
        KNotification::event(messageType,
                             options()->windowTitle(),
                             options()->text(),
                             options()->iconPixmap(),
                             KNotification::DefaultEvent | KNotification::CloseOnTimeout);
    }

    return true;
}

void KDEPlatformMessageDialogHelper::hide()
{
    m_box->hide();
}

#include "moc_kdeplatformmessagedialoghelper.cpp"
