/*  This file is part of the KDE libraries
 *  Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  Copyright 2014 Martin Klapetek <mklapetek@kde.org>
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

#include "kdeplatformfiledialoghelper.h"
#include "kdeplatformfiledialogbase_p.h"
#include "kdirselectdialog_p.h"

#include <kfilefiltercombo.h>
#include <kfilewidget.h>
#include <klocalizedstring.h>
#include <kdiroperator.h>
#include <KUrlComboBox>
#include <KSharedConfig>
#include <KWindowConfig>
#include <KProtocolInfo>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QWindow>

#include <QTextStream>
#include <QEventLoop>

namespace
{

/*
 * Map a Qt filter string into a KDE one.
 */
static QString qt2KdeFilter(const QStringList &f)
{
    QString               filter;
    QTextStream           str(&filter, QIODevice::WriteOnly);
    QStringList           list(f);
    list.replaceInStrings(QStringLiteral("/"), QStringLiteral("\\/"));
    QStringList::const_iterator it(list.constBegin()), end(list.constEnd());
    bool                  first = true;

    for (; it != end; ++it) {
        int ob = it->lastIndexOf('('),
            cb = it->lastIndexOf(')');

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
static QString kde2QtFilter(const QStringList &list, const QString &kde)
{
    QStringList::const_iterator it(list.constBegin()), end(list.constEnd());
    int                   pos;
    QString               sel;

    for (; it != end; ++it) {
        if (-1 != (pos = it->indexOf(kde)) && pos > 0 &&
                ('(' == (*it)[pos - 1] || ' ' == (*it)[pos - 1]) &&
                it->length() >= kde.length() + pos &&
                (')' == (*it)[pos + kde.length()] || ' ' == (*it)[pos + kde.length()])) {
            return *it;
        }
    }
    return QString();
}
}

KDEPlatformFileDialog::KDEPlatformFileDialog()
    : KDEPlatformFileDialogBase()
    , m_fileWidget(new KFileWidget(QUrl(), this))
{
    setLayout(new QVBoxLayout);
    connect(m_fileWidget, SIGNAL(filterChanged(QString)), SIGNAL(filterSelected(QString)));
    layout()->addWidget(m_fileWidget);

    m_buttons = new QDialogButtonBox(this);
    m_buttons->addButton(m_fileWidget->okButton(), QDialogButtonBox::AcceptRole);
    m_buttons->addButton(m_fileWidget->cancelButton(), QDialogButtonBox::RejectRole);
    connect(m_buttons, SIGNAL(rejected()), m_fileWidget, SLOT(slotCancel()));
    connect(m_fileWidget->okButton(), SIGNAL(clicked(bool)), m_fileWidget, SLOT(slotOk()));
    connect(m_fileWidget, SIGNAL(accepted()), m_fileWidget, SLOT(accept()));
    connect(m_fileWidget, SIGNAL(accepted()), SLOT(accept()));
    connect(m_fileWidget->cancelButton(), SIGNAL(clicked(bool)), SLOT(reject()));
    layout()->addWidget(m_buttons);
}

QUrl KDEPlatformFileDialog::directory()
{
    return m_fileWidget->baseUrl();
}

QList<QUrl> KDEPlatformFileDialog::selectedFiles()
{
    return m_fileWidget->selectedUrls();
}

void KDEPlatformFileDialog::selectFile(const QUrl &filename)
{
    QUrl dirUrl = filename.adjusted(QUrl::RemoveFilename);
    m_fileWidget->setUrl(dirUrl);
    m_fileWidget->setSelection(filename.fileName());
}

void KDEPlatformFileDialog::setViewMode(QFileDialogOptions::ViewMode view)
{
    switch (view) {
    case QFileDialogOptions::ViewMode::Detail:
        m_fileWidget->setViewMode(KFile::FileView::Detail);
        break;
    case QFileDialogOptions::ViewMode::List:
        m_fileWidget->setViewMode(KFile::FileView::Simple);
        break;
    default:
        m_fileWidget->setViewMode(KFile::FileView::Default);
        break;
    }
}

void KDEPlatformFileDialog::setFileMode(QFileDialogOptions::FileMode mode)
{
    switch (mode) {
    case QFileDialogOptions::FileMode::AnyFile:
        m_fileWidget->setMode(KFile::File);
        break;
    case QFileDialogOptions::FileMode::ExistingFile:
        m_fileWidget->setMode(KFile::Mode::File | KFile::Mode::ExistingOnly);
        break;
    case QFileDialogOptions::FileMode::Directory:
        m_fileWidget->setMode(KFile::Mode::Directory);
        break;
    case QFileDialogOptions::FileMode::ExistingFiles:
        m_fileWidget->setMode(KFile::Mode::Files | KFile::Mode::ExistingOnly);
        break;
    default:
        m_fileWidget->setMode(KFile::File);
        break;
    }
}

void KDEPlatformFileDialog::setCustomLabel(QFileDialogOptions::DialogLabel label, const QString &text)
{
    if (label == QFileDialogOptions::Accept) { // OK button
        m_fileWidget->okButton()->setText(text);
    } else if (label == QFileDialogOptions::Reject) { // Cancel button
        m_fileWidget->cancelButton()->setText(text);
    } else if (label == QFileDialogOptions::LookIn) { // Location label
        m_fileWidget->setLocationLabel(text);
    }
}

QString KDEPlatformFileDialog::selectedNameFilter()
{
    return m_fileWidget->filterWidget()->currentFilter();
}

void KDEPlatformFileDialog::selectNameFilter(const QString &filter)
{
    m_fileWidget->filterWidget()->setCurrentFilter(filter);
}

void KDEPlatformFileDialog::setDirectory(const QUrl &directory)
{
    m_fileWidget->setUrl(directory);
}

bool KDEPlatformFileDialogHelper::isSupportedUrl(const QUrl& url) const
{
    return KProtocolInfo::protocols().contains(url.scheme());
}

////////////////////////////////////////////////

KDEPlatformFileDialogHelper::KDEPlatformFileDialogHelper()
    : QPlatformFileDialogHelper()
    , m_dialog(new KDEPlatformFileDialog)
{
    connect(m_dialog, SIGNAL(closed()), SLOT(saveSize()));
    connect(m_dialog, SIGNAL(finished(int)), SLOT(saveSize()));
    connect(m_dialog, SIGNAL(currentChanged(QUrl)), SIGNAL(currentChanged(QUrl)));
    connect(m_dialog, SIGNAL(directoryEntered(QUrl)), SIGNAL(directoryEntered(QUrl)));
    connect(m_dialog, SIGNAL(fileSelected(QUrl)), SIGNAL(fileSelected(QUrl)));
    connect(m_dialog, SIGNAL(filesSelected(QList<QUrl>)), SIGNAL(filesSelected(QList<QUrl>)));
    connect(m_dialog, SIGNAL(filterSelected(QString)), SIGNAL(filterSelected(QString)));
    connect(m_dialog, SIGNAL(accepted()), SIGNAL(accept()));
    connect(m_dialog, SIGNAL(rejected()), SIGNAL(reject()));
}

KDEPlatformFileDialogHelper::~KDEPlatformFileDialogHelper()
{
    saveSize();
    delete m_dialog;
}

void KDEPlatformFileDialogHelper::initializeDialog()
{
    if (options()->testOption(QFileDialogOptions::ShowDirsOnly)) {
        KDirSelectDialog *dialog = new KDirSelectDialog(m_dialog->directory());
        delete m_dialog;
        m_dialog = dialog;
        connect(m_dialog, SIGNAL(accepted()), SIGNAL(accept()));
        connect(m_dialog, SIGNAL(rejected()), SIGNAL(reject()));
    } else {
        // needed for accessing m_fileWidget
        KDEPlatformFileDialog *dialog = qobject_cast<KDEPlatformFileDialog*>(m_dialog);
        dialog->m_fileWidget->setOperationMode(options()->acceptMode() == QFileDialogOptions::AcceptOpen ? KFileWidget::Opening : KFileWidget::Saving);
        if (options()->windowTitle().isEmpty()) {
            dialog->setWindowTitle(options()->acceptMode() == QFileDialogOptions::AcceptOpen ? i18n("Opening...") : i18n("Saving..."));
        } else {
            dialog->setWindowTitle(options()->windowTitle());
        }
        setDirectory(options()->initialDirectory());
        //dialog->setViewMode(options()->viewMode()); // don't override our options, fixes remembering the chosen view mode and sizes!
        dialog->setFileMode(options()->fileMode());

        // custom labels
        if (options()->isLabelExplicitlySet(QFileDialogOptions::Accept)) { // OK button
            dialog->setCustomLabel(QFileDialogOptions::Accept, options()->labelText(QFileDialogOptions::Accept));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::Reject)) { // Cancel button
            dialog->setCustomLabel(QFileDialogOptions::Reject, options()->labelText(QFileDialogOptions::Reject));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::LookIn)) { // Location label
            dialog->setCustomLabel(QFileDialogOptions::LookIn, options()->labelText(QFileDialogOptions::LookIn));
        }

        // MIME filters
        QStringList filters = options()->mimeTypeFilters();
        if (!filters.isEmpty()) {
            dialog->m_fileWidget->setMimeFilter(filters);
        }

        // name filters
        QStringList nameFilters = options()->nameFilters();
        if (!nameFilters.isEmpty()) {
            dialog->m_fileWidget->setFilter(qt2KdeFilter(nameFilters));
            if (!options()->initiallySelectedNameFilter().isEmpty()) {
                selectNameFilter(options()->initiallySelectedNameFilter());
            }
        }

        // overwrite option
        if (options()->testOption(QFileDialogOptions::FileDialogOption::DontConfirmOverwrite)) {
            dialog->m_fileWidget->setConfirmOverwrite(false);
        }
    }
}

void KDEPlatformFileDialogHelper::exec()
{
    m_dialog->hide(); // ensure dialog is not shown (exec would block input)
    m_dialog->winId(); // ensure there's a window created
    KSharedConfig::Ptr conf = KSharedConfig::openConfig();
    KWindowConfig::restoreWindowSize(m_dialog->windowHandle(), conf->group("FileDialogSize"));
    // NOTICE: QWindow::setGeometry() does NOT impact the backing QWidget geometry even if the platform
    // window was created -> QTBUG-40584. We therefore copy the size here.
    // TODO: remove once this was resolved in QWidget QPA
    m_dialog->resize(m_dialog->windowHandle()->size());
    m_dialog->exec();
}

void KDEPlatformFileDialogHelper::hide()
{
    m_dialog->hide();
}

void KDEPlatformFileDialogHelper::saveSize()
{
    KSharedConfig::Ptr conf = KSharedConfig::openConfig();
    KConfigGroup group = conf->group("FileDialogSize");
    KWindowConfig::saveWindowSize(m_dialog->windowHandle(), group);
}

bool KDEPlatformFileDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(parent)
    initializeDialog();
    m_dialog->setWindowFlags(windowFlags);
    m_dialog->setWindowModality(windowModality);
    m_dialog->show();
    KSharedConfig::Ptr conf = KSharedConfig::openConfig();
    KWindowConfig::restoreWindowSize(m_dialog->windowHandle(), conf->group("FileDialogSize"));
    return true;
}

QList<QUrl> KDEPlatformFileDialogHelper::selectedFiles() const
{
    return m_dialog->selectedFiles();
}

QString KDEPlatformFileDialogHelper::selectedNameFilter() const
{
    return kde2QtFilter(options()->nameFilters(), m_dialog->selectedNameFilter());
}

QUrl KDEPlatformFileDialogHelper::directory() const
{
    return m_dialog->directory();
}

void KDEPlatformFileDialogHelper::selectFile(const QUrl &filename)
{
    m_dialog->selectFile(filename);
}

void KDEPlatformFileDialogHelper::setDirectory(const QUrl &directory)
{
    m_dialog->setDirectory(directory);
}

void KDEPlatformFileDialogHelper::selectNameFilter(const QString &filter)
{
    m_dialog->selectNameFilter(qt2KdeFilter(QStringList(filter)));
}

void KDEPlatformFileDialogHelper::setFilter()
{
}

bool KDEPlatformFileDialogHelper::defaultNameFilterDisables() const
{
    return false;
}
