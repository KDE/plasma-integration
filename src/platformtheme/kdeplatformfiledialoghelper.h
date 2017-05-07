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

#ifndef KDEPLATFORMFILEDIALOGHELPER_H
#define KDEPLATFORMFILEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>
#include "kdeplatformfiledialogbase_p.h"

class KFileWidget;
class QDialogButtonBox;
class KDEPlatformFileDialog : public KDEPlatformFileDialogBase
{
    Q_OBJECT
public:
    friend class KDEPlatformFileDialogHelper;

    explicit KDEPlatformFileDialog();
    QUrl directory() override;
    void selectMimeTypeFilter(const QString &filter) override;
    void selectNameFilter(const QString &filter) override;
    void setDirectory(const QUrl &directory) override;
    void selectFile(const QUrl &filename) override;
    void setViewMode(QFileDialogOptions::ViewMode view);
    void setFileMode(QFileDialogOptions::FileMode mode);
    void setCustomLabel(QFileDialogOptions::DialogLabel label, const QString & text);
    QString selectedMimeTypeFilter() override;
    QString selectedNameFilter() override;
    QList<QUrl> selectedFiles() override;

protected:
    KFileWidget *m_fileWidget;
};

class KDEPlatformFileDialogHelper : public QPlatformFileDialogHelper
{
    Q_OBJECT
public:
    KDEPlatformFileDialogHelper();
    virtual ~KDEPlatformFileDialogHelper();

    void initializeDialog();

    bool defaultNameFilterDisables() const override;
    QUrl directory() const override;
    QList<QUrl> selectedFiles() const override;
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    QString selectedMimeTypeFilter() const override;
    void selectMimeTypeFilter(const QString &filter) override;
#endif
    QString selectedNameFilter() const override;
    void selectNameFilter(const QString &filter) override;
    void selectFile(const QUrl &filename) override;
    void setFilter() override;
    void setDirectory(const QUrl &directory) override;
    bool isSupportedUrl(const QUrl& url) const override;

    void exec() override;
    void hide() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;

private Q_SLOTS:
    void saveSize();

private:
    void restoreSize();
    KDEPlatformFileDialogBase *m_dialog;
};

#endif // KDEPLATFORMFILEDIALOGHELPER_H
