/*  This file is part of the KDE libraries
 *  Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KDEPLATFORMFILEDIALOGBASE_H
#define KDEPLATFORMFILEDIALOGBASE_H

#include <QDialog>
#include <QUrl>

class KFileWidget;
class QDialogButtonBox;
class KDEPlatformFileDialogBase : public QDialog
{
    Q_OBJECT
public:
    friend class KDEPlatformFileDialogHelper;

    explicit KDEPlatformFileDialogBase();
    virtual QUrl directory() = 0;
    virtual void selectNameFilter(const QString &filter) = 0;
    virtual void setDirectory(const QUrl &directory) = 0;
    virtual void selectFile(const QUrl &filename) = 0;
    virtual QString selectedNameFilter() = 0;
    virtual QList<QUrl> selectedFiles() = 0;

Q_SIGNALS:
    void fileSelected(const QUrl &file);
    void filesSelected(const QList<QUrl> &files);
    void currentChanged(const QUrl &path);
    void directoryEntered(const QUrl &directory);
    void filterSelected(const QString &filter);

protected:
    QDialogButtonBox *m_buttons;

};

#endif
