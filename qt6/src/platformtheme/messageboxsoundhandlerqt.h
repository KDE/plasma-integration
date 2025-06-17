// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>

#pragma once

#include <QObject>

class MessageBoxSoundHandlerQt : public QObject
{
    Q_OBJECT

public:
    explicit MessageBoxSoundHandlerQt(QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;
};
