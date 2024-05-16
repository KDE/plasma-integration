// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QAccessible>
#include <QDBusConnection>
#include <QDBusMessage>

class A11yExtension : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.a11y.atspi.extension")
public:
    A11yExtension();

public Q_SLOTS:
    [[nodiscard]] QVariantHash accessibleProperties(const QString &accessibleId);
    [[nodiscard]] QString identifyWindowByTitle(const QString &accessibleId);
    void resetWindowTitle(const QString &uuid);

private:
    QDBusConnection m_connection;
    QMap<QWindow *, QString> m_originalTitles;
};
