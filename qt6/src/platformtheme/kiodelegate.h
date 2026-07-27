// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022-2026 Harald Sitter <sitter@kde.org>

#pragma once

#include <KIO/JobUiDelegate>
#include <KIO/JobUiDelegateFactory>

class KIOUiDelegate : public KIO::JobUiDelegate
{
public:
    explicit KIOUiDelegate(KJobUiDelegate::Flags flags = AutoHandlingDisabled, QWidget *window = nullptr);
    [[nodiscard]] bool setJob(KJob *job) override;
    void showErrorMessage() override;
    void slotWarning([[maybe_unused]] KJob *job, const QString &message) override;

private:
    friend class MessageDispatch;
    QString m_title; // cache title for notifications
};

class KIOUiFactory : public KIO::JobUiDelegateFactory
{
public:
    KIOUiFactory() = default; // JobUiDelegateFactory has a protected ctor, we cannot `using` delegate to it.

    KJobUiDelegate *createDelegate() const override;
    KJobUiDelegate *createDelegate(KJobUiDelegate::Flags flags, QWidget *window) const override;
};
