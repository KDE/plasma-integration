/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kdeplatformfontdialoghelper.h"

#include <QFont>
#include <QFontDialog>
#include <QWindow>

KDEPlatformFontDialogHelper::KDEPlatformFontDialogHelper()
    : QPlatformFontDialogHelper()
{
    m_dialog = std::make_unique<QFontDialog>();

    connect(m_dialog.get(), &QFontDialog::currentFontChanged, this, &KDEPlatformFontDialogHelper::currentFontChanged);
    connect(m_dialog.get(), &QFontDialog::fontSelected, this, &KDEPlatformFontDialogHelper::fontSelected);
    connect(m_dialog.get(), &QFontDialog::accepted, this, &KDEPlatformFontDialogHelper::accept);
    connect(m_dialog.get(), &QFontDialog::rejected, this, &KDEPlatformFontDialogHelper::reject);
}

void KDEPlatformFontDialogHelper::setup()
{
    m_dialog->setWindowTitle(options()->windowTitle());

    auto opts = options()->options();
    opts |= QFontDialogOptions::DontUseNativeDialog;
    m_dialog->setOptions(QFlags<QFontDialog::FontDialogOption>::fromInt(opts));
}

void KDEPlatformFontDialogHelper::exec()
{
    setup();
    m_dialog->exec();
}

bool KDEPlatformFontDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    setup();
    m_dialog->setWindowFlags(windowFlags);
    m_dialog->setWindowModality(windowModality);
    m_dialog->winId();
    m_dialog->windowHandle()->setTransientParent(parent);

    m_dialog->show();
    return true;
}

void KDEPlatformFontDialogHelper::hide()
{
    m_dialog->hide();
}

void KDEPlatformFontDialogHelper::setCurrentFont(const QFont &font)
{
    m_dialog->setCurrentFont(font);
}

QFont KDEPlatformFontDialogHelper::currentFont() const
{
    return m_dialog->currentFont();
}
