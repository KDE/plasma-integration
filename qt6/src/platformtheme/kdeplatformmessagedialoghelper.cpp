// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "kdeplatformmessagedialoghelper.h"

#include <KWindowSystem>

#include <QCheckBox>
#include <QMessageBox>
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
    connect(m_box, &QDialog::accepted, this, &QPlatformDialogHelper::accept);
    connect(m_box, &QDialog::rejected, this, &QPlatformDialogHelper::reject);

    for (const auto &button : options()->customButtons()) {
        m_box->addButton(button.label, static_cast<QMessageBox::ButtonRole>(button.role));
    }

    for (int i = StandardButton::FirstButton; i < StandardButton::LastButton; i <<= 1) {
        if (i & options()->standardButtons()) {
            m_box->addButton(static_cast<QMessageBox::StandardButton>(i));
        }
    }

    connect(m_box, &QMessageBox::buttonClicked, this, [this](const auto &button) {
        Q_EMIT clicked(static_cast<QPlatformDialogHelper::StandardButton>(m_box->standardButton(button)),
                       static_cast<QPlatformDialogHelper::ButtonRole>(m_box->buttonRole(button)));
    });

    m_box->open();
    return true;
}

void KDEPlatformMessageDialogHelper::hide()
{
    m_box->hide();
}

#include "moc_kdeplatformmessagedialoghelper.cpp"
