// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QColorDialog>
#include <qpa/qplatformdialoghelper.h>

class KDEPlatformColorDialogHelper : public QPlatformColorDialogHelper
{
    Q_OBJECT
public:
    explicit KDEPlatformColorDialogHelper();
    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;

    void setCurrentColor(const QColor &color) override;
    QColor currentColor() const override;

private:
    std::unique_ptr<QColorDialog> m_dialog = nullptr;
};
