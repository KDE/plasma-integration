// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <qpa/qplatformdialoghelper.h>

#include <QPointer>

class QMessageBox;
class QAbstractButton;

class KDEPlatformMessageDialogHelper : public QPlatformMessageDialogHelper
{
    Q_OBJECT
public:
    using QPlatformMessageDialogHelper::QPlatformMessageDialogHelper;
    ~KDEPlatformMessageDialogHelper() override;
    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parent) override;
    void hide() override;

private:
    // QPointer: the box deletes itself on close (WA_DeleteOnClose), but
    // QDialog calls hide() on this helper again afterwards, from its destructor
    QPointer<QMessageBox> m_box;
    QMap<QAbstractButton *, int> m_customButtonIds;
};
