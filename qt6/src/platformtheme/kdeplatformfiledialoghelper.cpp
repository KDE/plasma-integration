/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kdeplatformfiledialoghelper.h"
#include "kdirselectdialog_p.h"

#include <KFileFilter>
#include <KIO/StatJob>
#include <KJobWidgets>
#include <KProtocolInfo>
#include <KSharedConfig>
#include <KWindowConfig>
#include <kdiroperator.h>
#include <kfilefiltercombo.h>
#include <kfilewidget.h>
#include <kio_version.h>
#include <klocalizedstring.h>

#include <QDialogButtonBox>
#include <QMimeDatabase>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWindow>
namespace
{
static QString fileFilter2NameFilter(const KFileFilter &filter)
{
    if (filter.isEmpty()) {
        return QString();
    }

    return QStringLiteral("%1 (%2)").arg(filter.label(), filter.filePatterns().join(" "));
}
}

KDEPlatformFileDialog::KDEPlatformFileDialog()
    : KDEPlatformFileDialogBase()
    , m_fileWidget(new KFileWidget(QUrl(), this))
{
    auto v = new QVBoxLayout;
    v->setContentsMargins({});
    setLayout(v);
    connect(m_fileWidget, &KFileWidget::filterChanged, this, [this](const KFileFilter &filter) {
        if (filter.isEmpty()) {
            return;
        }

        if (!filter.filePatterns().isEmpty()) {
            Q_EMIT filterSelected(fileFilter2NameFilter(filter));
        } else {
            Q_ASSERT(!filter.mimePatterns().isEmpty());
            QMimeDatabase db;
            QMimeType mimeType = db.mimeTypeForName(filter.mimePatterns().first());
            Q_EMIT filterSelected(mimeType.filterString());
        }
    });

    layout()->addWidget(m_fileWidget);
    m_fileWidget->okButton()->show();
    m_fileWidget->cancelButton()->show();

    connect(this, &QDialog::rejected, m_fileWidget, &KFileWidget::slotCancel);
    // Also call the cancel function when the dialog is closed via the escape key
    // or titlebar close button to make sure we always save the view config
    connect(this, &KDEPlatformFileDialog::rejected, m_fileWidget, &KFileWidget::slotCancel);
    connect(m_fileWidget->okButton(), &QAbstractButton::clicked, m_fileWidget, &KFileWidget::slotOk);
    connect(m_fileWidget, &KFileWidget::accepted, m_fileWidget, &KFileWidget::accept);
    connect(m_fileWidget, &KFileWidget::accepted, this, &QDialog::accept);
    connect(m_fileWidget->cancelButton(), &QAbstractButton::clicked, this, &QDialog::reject);
    connect(m_fileWidget->dirOperator(), &KDirOperator::urlEntered, this, &KDEPlatformFileDialogBase::directoryEntered);
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
    const QUrl dirUrl = filename.adjusted(QUrl::RemoveFilename);
    m_fileWidget->setUrl(dirUrl);
    m_fileWidget->setSelectedUrl(filename);
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
        m_fileWidget->setMode(KFile::Mode::Directory | KFile::Mode::ExistingOnly);
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

QString KDEPlatformFileDialog::selectedMimeTypeFilter()
{
    const QStringList mimeTypes = m_fileWidget->filterWidget()->currentFilter().mimePatterns();

    if (mimeTypes.length() == 1) {
        return mimeTypes.first();
    }

    if (selectedFiles().isEmpty()) {
        return QString();
    }

    // Works for both KFile::File and KFile::Files modes.
    return QMimeDatabase().mimeTypeForUrl(selectedFiles().at(0)).name();
}

QString KDEPlatformFileDialog::selectedNameFilter()
{
    return fileFilter2NameFilter(m_fileWidget->filterWidget()->currentFilter());
}

QString KDEPlatformFileDialog::currentFilterText()
{
    return m_fileWidget->filterWidget()->currentText();
}

void KDEPlatformFileDialog::selectMimeTypeFilter(const QString &filter)
{
    m_fileWidget->filterWidget()->setCurrentFilter(KFileFilter::fromMimeType(filter));
}

void KDEPlatformFileDialog::selectNameFilter(const KFileFilter &filter)
{
    m_fileWidget->filterWidget()->setCurrentFilter(filter);
}

void KDEPlatformFileDialog::setDirectory(const QUrl &directory)
{
    if (!directory.isLocalFile()) {
        // Short-circuit: Avoid stat if the effective URL hasn't changed
        if (directory == m_fileWidget->baseUrl()) {
            return;
        }

        // Qt can not determine if the remote URL points to a file or a
        // directory, that is why options()->initialDirectory() always returns
        // the full URL.
        KIO::StatJob *job = KIO::stat(directory);
        KJobWidgets::setWindow(job, this);
        if (job->exec()) {
            KIO::UDSEntry entry = job->statResult();
            if (!entry.isDir()) {
                // this is probably a file remove the file part
                m_fileWidget->setUrl(directory.adjusted(QUrl::RemoveFilename));
                m_fileWidget->setSelectedUrl(directory);
            } else {
                m_fileWidget->setUrl(directory);
            }
        }
    } else {
        m_fileWidget->setUrl(directory);
    }
}

bool KDEPlatformFileDialogHelper::isSupportedUrl(const QUrl &url) const
{
    return KProtocolInfo::protocols().contains(url.scheme());
}

////////////////////////////////////////////////

KDEPlatformFileDialogHelper::KDEPlatformFileDialogHelper()
    : QPlatformFileDialogHelper()
    , m_dialog(new KDEPlatformFileDialog)
{
    connect(m_dialog, &KDEPlatformFileDialogBase::closed, this, &KDEPlatformFileDialogHelper::saveSize);
    connect(m_dialog, &QDialog::finished, this, &KDEPlatformFileDialogHelper::saveSize);
    connect(m_dialog, &KDEPlatformFileDialogBase::currentChanged, this, &QPlatformFileDialogHelper::currentChanged);
    connect(m_dialog, &KDEPlatformFileDialogBase::directoryEntered, this, &QPlatformFileDialogHelper::directoryEntered);
    connect(m_dialog, &KDEPlatformFileDialogBase::fileSelected, this, &QPlatformFileDialogHelper::fileSelected);
    connect(m_dialog, &KDEPlatformFileDialogBase::filesSelected, this, &QPlatformFileDialogHelper::filesSelected);
    connect(m_dialog, &KDEPlatformFileDialogBase::filterSelected, this, &QPlatformFileDialogHelper::filterSelected);
    connect(m_dialog, &QDialog::accepted, this, &QPlatformDialogHelper::accept);
    connect(m_dialog, &QDialog::rejected, this, &QPlatformDialogHelper::reject);
}

KDEPlatformFileDialogHelper::~KDEPlatformFileDialogHelper()
{
    saveSize();
    delete m_dialog;
}

void KDEPlatformFileDialogHelper::initializeDialog()
{
    m_dialogInitialized = true;

    if (options()->testOption(QFileDialogOptions::ShowDirsOnly) || options()->fileMode() == QFileDialogOptions::Directory
        || options()->fileMode() == QFileDialogOptions::DirectoryOnly) {
        m_dialog->deleteLater();
        KDirSelectDialog *dialog = new KDirSelectDialog(options()->initialDirectory());
        m_dialog = dialog;
        connect(dialog, &QDialog::accepted, this, &QPlatformDialogHelper::accept);
        connect(dialog, &QDialog::rejected, this, &QPlatformDialogHelper::reject);
        if (options()->isLabelExplicitlySet(QFileDialogOptions::Accept)) { // OK button
            dialog->setOkButtonText(options()->labelText(QFileDialogOptions::Accept));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::Reject)) { // Cancel button
            dialog->setCancelButtonText(options()->labelText(QFileDialogOptions::Reject));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::LookIn)) { // Location label
            // Not implemented yet.
        }

        if (!options()->windowTitle().isEmpty())
            m_dialog->setWindowTitle(options()->windowTitle());
    } else {
        // needed for accessing m_fileWidget
        KDEPlatformFileDialog *dialog = qobject_cast<KDEPlatformFileDialog *>(m_dialog);
        dialog->m_fileWidget->setOperationMode(options()->acceptMode() == QFileDialogOptions::AcceptOpen ? KFileWidget::Opening : KFileWidget::Saving);
        if (options()->windowTitle().isEmpty()) {
            dialog->setWindowTitle(options()->acceptMode() == QFileDialogOptions::AcceptOpen ? i18nc("@title:window", "Open File")
                                                                                             : i18nc("@title:window", "Save File"));
        } else {
            dialog->setWindowTitle(options()->windowTitle());
        }
        if (!m_directorySet) {
            setDirectory(options()->initialDirectory());
        }
        // dialog->setViewMode(options()->viewMode()); // don't override our options, fixes remembering the chosen view mode and sizes!
        dialog->setFileMode(options()->fileMode());

        // custom labels
        if (options()->isLabelExplicitlySet(QFileDialogOptions::Accept)) { // OK button
            dialog->setCustomLabel(QFileDialogOptions::Accept, options()->labelText(QFileDialogOptions::Accept));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::Reject)) { // Cancel button
            dialog->setCustomLabel(QFileDialogOptions::Reject, options()->labelText(QFileDialogOptions::Reject));
        } else if (options()->isLabelExplicitlySet(QFileDialogOptions::LookIn)) { // Location label
            dialog->setCustomLabel(QFileDialogOptions::LookIn, options()->labelText(QFileDialogOptions::LookIn));
        }

        const QStringList mimeFilters = options()->mimeTypeFilters();
        const QStringList nameFilters = options()->nameFilters();
        if (!mimeFilters.isEmpty()) {
            QString defaultMimeFilter;
            if (options()->acceptMode() == QFileDialogOptions::AcceptSave) {
                defaultMimeFilter = options()->initiallySelectedMimeTypeFilter();
                if (defaultMimeFilter.isEmpty()) {
                    defaultMimeFilter = mimeFilters.at(0);
                }
            }
            dialog->m_fileWidget->setFilters(KFileFilter::fromMimeTypes(mimeFilters), KFileFilter::fromMimeType(defaultMimeFilter));

            if (mimeFilters.contains(QStringLiteral("inode/directory")))
                dialog->m_fileWidget->setMode(dialog->m_fileWidget->mode() | KFile::Directory);
        } else if (!nameFilters.isEmpty()) {
            QList<KFileFilter> fileFilters;
            fileFilters.reserve(nameFilters.size());

            for (const QString &nameFilter : nameFilters) {
                fileFilters << qtFilterToKFileFilter(nameFilter);
            }

            dialog->m_fileWidget->setFilters(fileFilters);
        }

        if (!options()->initiallySelectedMimeTypeFilter().isEmpty()) {
            selectMimeTypeFilter(options()->initiallySelectedMimeTypeFilter());
        } else if (!options()->initiallySelectedNameFilter().isEmpty()) {
            selectNameFilter(options()->initiallySelectedNameFilter());
        }

        // overwrite option
        if (options()->testOption(QFileDialogOptions::FileDialogOption::DontConfirmOverwrite)) {
            dialog->m_fileWidget->setConfirmOverwrite(false);
        } else if (options()->acceptMode() == QFileDialogOptions::AcceptSave) {
            dialog->m_fileWidget->setConfirmOverwrite(true);
        }

        const QStringList schemes = options()->supportedSchemes();
        dialog->m_fileWidget->setSupportedSchemes(schemes);
    }
}

KFileFilter KDEPlatformFileDialogHelper::qtFilterToKFileFilter(const QString &qtFilter) const
{
    QString name = qtFilter.left(qtFilter.lastIndexOf(u'(')).trimmed();
    const QStringList extensions = cleanFilterList(qtFilter);

    if (name.isEmpty()) {
        name = extensions.join(QLatin1Char(' '));
    }

    return KFileFilter(name, extensions, {});
}

void KDEPlatformFileDialogHelper::exec()
{
    restoreSize();
    m_dialog->exec();
}

void KDEPlatformFileDialogHelper::hide()
{
    m_dialog->hide();
}

void KDEPlatformFileDialogHelper::saveSize()
{
    KSharedConfig::Ptr conf = KSharedConfig::openStateConfig();
    KConfigGroup group = conf->group("FileDialogSize");
    KWindowConfig::saveWindowSize(m_dialog->windowHandle(), group);
}

void KDEPlatformFileDialogHelper::restoreSize()
{
    m_dialog->winId(); // ensure there's a window created
    KSharedConfig::Ptr oldConf = KSharedConfig::openConfig();

    auto conf = KSharedConfig::openStateConfig();
    auto stateGroup = conf->group("FileDialogSize");
    oldConf->group("FileDialogSize").moveValuesTo(stateGroup);

    // see the note below
    m_dialog->windowHandle()->resize(m_dialog->sizeHint());

    KWindowConfig::restoreWindowSize(m_dialog->windowHandle(), stateGroup);
    // NOTICE: QWindow::setGeometry() does NOT impact the backing QWidget geometry even if the platform
    // window was created -> QTBUG-40584. We therefore copy the size here.
    // TODO: remove once this was resolved in QWidget QPA
    m_dialog->resize(m_dialog->windowHandle()->size());
}

bool KDEPlatformFileDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    initializeDialog();
    m_dialog->setWindowFlags(windowFlags);
    m_dialog->setWindowModality(windowModality);
    restoreSize();
    m_dialog->windowHandle()->setTransientParent(parent);
    m_dialog->show();
    return true;
}

QVariant KDEPlatformFileDialogHelper::styleHint(StyleHint hint) const
{
    if (hint == DialogIsQtWindow) {
        return true;
    }

    return QPlatformDialogHelper::styleHint(hint);
}

QList<QUrl> KDEPlatformFileDialogHelper::selectedFiles() const
{
    return m_dialog->selectedFiles();
}

QString KDEPlatformFileDialogHelper::selectedMimeTypeFilter() const
{
    return m_dialog->selectedMimeTypeFilter();
}

void KDEPlatformFileDialogHelper::selectMimeTypeFilter(const QString &filter)
{
    m_dialog->selectMimeTypeFilter(filter);
}

QString KDEPlatformFileDialogHelper::selectedNameFilter() const
{
    return m_dialog->selectedNameFilter();
}

QUrl KDEPlatformFileDialogHelper::directory() const
{
    return m_dialog->directory();
}

void KDEPlatformFileDialogHelper::selectFile(const QUrl &filename)
{
    m_dialog->selectFile(filename);
    m_fileSelected = true;
}

void KDEPlatformFileDialogHelper::setDirectory(const QUrl &directory)
{
    if (!directory.isEmpty()) {
        m_dialog->setDirectory(directory);
        m_directorySet = true;
    }
}

void KDEPlatformFileDialogHelper::selectNameFilter(const QString &filter)
{
    m_dialog->selectNameFilter(qtFilterToKFileFilter(filter));
}

void KDEPlatformFileDialogHelper::setFilter()
{
}

bool KDEPlatformFileDialogHelper::defaultNameFilterDisables() const
{
    return false;
}

#include "moc_kdeplatformfiledialoghelper.cpp"
