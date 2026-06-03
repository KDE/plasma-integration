// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kdeplatformcolordialoghelper.h"

#include <QColor>
#include <QColorDialog>
#include <QWindow>

KDEPlatformColorDialogHelper::KDEPlatformColorDialogHelper()
    : QPlatformColorDialogHelper()
    , m_dialog(std::make_unique<QColorDialog>())
{
}

void KDEPlatformColorDialogHelper::setCurrentColor(const QColor &color)
{
    m_dialog->setCurrentColor(color);
}

QColor KDEPlatformColorDialogHelper::currentColor() const
{
    return m_dialog->currentColor();
}

void KDEPlatformColorDialogHelper::exec()
{
    m_dialog->exec();
}

bool KDEPlatformColorDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    m_dialog->setWindowTitle(options()->windowTitle());
    m_dialog->setWindowFlags(windowFlags);
    m_dialog->setWindowModality(windowModality);
    m_dialog->winId();
    m_dialog->windowHandle()->setTransientParent(parent);
    m_dialog->setOption(QColorDialog::ColorDialogOption::DontUseNativeDialog);

    connect(m_dialog.get(), &QColorDialog::accepted, this, &KDEPlatformColorDialogHelper::accept);
    connect(m_dialog.get(), &QColorDialog::rejected, this, &KDEPlatformColorDialogHelper::reject);
    connect(m_dialog.get(), &QColorDialog::currentColorChanged, this, &KDEPlatformColorDialogHelper::currentColorChanged);

    // HACK: Delayed to work around QTBUG-144324
    QMetaObject::invokeMethod(m_dialog.get(), &QDialog::show, Qt::QueuedConnection);

    return true;
}

void KDEPlatformColorDialogHelper::hide()
{
    m_dialog->hide();
}

#include "moc_kdeplatformcolordialoghelper.cpp"
