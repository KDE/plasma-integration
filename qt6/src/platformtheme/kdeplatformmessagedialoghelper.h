// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <qpa/qplatformdialoghelper.h>

class QMessageBox;

class KDEPlatformMessageDialogHelper : public QPlatformMessageDialogHelper
{
    Q_OBJECT
public:
    using QPlatformMessageDialogHelper::QPlatformMessageDialogHelper;
    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parent) override;
    void hide() override;

private:
    QMessageBox *m_box = nullptr;
};
