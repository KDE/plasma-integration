/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QIODevice>
#include <QString>
#include <QTextStream>

namespace
{
/*
 * Map a Qt filter string into a KDE one.
 */
inline QString qt2KdeFilter(const QStringList &f)
{
    QString filter;
    QTextStream str(&filter, QIODevice::WriteOnly);
    QStringList list(f);
    list.replaceInStrings(QStringLiteral("/"), QStringLiteral("\\/"));
    QStringList::const_iterator it(list.constBegin()), end(list.constEnd());
    bool first = true;

    for (; it != end; ++it) {
        int ob = it->lastIndexOf(QLatin1Char('(')), cb = it->lastIndexOf(QLatin1Char(')'));

        if (-1 != cb && ob < cb) {
            if (first) {
                first = false;
            } else {
                str << '\n';
            }
            str << it->mid(ob + 1, (cb - ob) - 1) << '|' << it->mid(0, ob);
        }
    }

    return filter;
}

/*
 * Map a KDE filter string into a Qt one.
 */
inline QString kde2QtFilter(const QStringList &list, const QString &kde, const QString &filterText)
{
    QStringList::const_iterator it(list.constBegin()), end(list.constEnd());
    int pos;

    for (; it != end; ++it) {
        if (-1 != (pos = it->indexOf(kde)) && pos > 0 && (QLatin1Char('(') == (*it)[pos - 1] || QLatin1Char(' ') == (*it)[pos - 1])
            && it->length() >= kde.length() + pos && (QLatin1Char(')') == (*it)[pos + kde.length()] || QLatin1Char(' ') == (*it)[pos + kde.length()])
            && (filterText.isEmpty() || it->startsWith(filterText))) {
            return *it;
        }
    }
    return QString();
}
}
