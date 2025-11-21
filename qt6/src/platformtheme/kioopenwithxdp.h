// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022-2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QDBusPendingReply>
#include <QPointer>

#include <KConfigGroup>
#include <KIO/OpenWithHandlerInterface>

class KIOOpenWithXDP : public KIO::OpenWithHandlerInterface
{
    Q_OBJECT
public:
    explicit KIOOpenWithXDP(QWidget *parentWidget, QObject *parent = nullptr);
    void promptUserForApplication(KJob *job, const QList<QUrl> &urls, const QString &mimeType) override;

private Q_SLOTS:
    void onApplicationChosen(uint responseCode, const QVariantMap &resultMap);

private:
    QPointer<QWidget> m_parentWidget;
};
