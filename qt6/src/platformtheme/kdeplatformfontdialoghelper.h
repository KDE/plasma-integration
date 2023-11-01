/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QFontDialog>
#include <qpa/qplatformdialoghelper.h>

class KDEPlatformFontDialogHelper : public QPlatformFontDialogHelper
{
    Q_OBJECT

public:
    KDEPlatformFontDialogHelper();
    ~KDEPlatformFontDialogHelper() override = default;

    void exec() override;

    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;

    void setCurrentFont(const QFont &) override;

    QFont currentFont() const override;

private:
    void setup();
    std::unique_ptr<QFontDialog> m_dialog;
};
