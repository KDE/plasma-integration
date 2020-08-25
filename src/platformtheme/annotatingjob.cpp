#include <QApplication>
#include <QtConcurrent>
#include <QFileSystemWatcher>
#include <QDebug>

#include <sys/types.h>
#include <sys/xattr.h>

#include "annotatingjob.h"

AnnotatingJob::AnnotatingJob(const QList<QUrl>& jobs) : KJob(), m_jobs(jobs), m_watcher(new QFileSystemWatcher(this))
{
}

struct XattrInfo {
    const char* xattrName;
    QByteArray xattrValue;

    bool applyToFile(const QString &file) {
        // setxattr returns -1 on failure
        // so return true if it's not -1 (success)
        // and false if it is -1 (failure)
        return setxattr(file.toLocal8Bit().data(), this->xattrName, this->xattrValue.data(), this->xattrValue.length(), 0) != -1;
    }
};

static XattrInfo GetXAttrInfo() {
    // Let's hope our application is well behaved and set a desktop file name
    auto value = QApplication::desktopFileName();
    auto name = "user.org.kde.plasma.file-created-by";

    if (value.length() == 0) {
        // They didn't behave, so let's use the executable path as a workaround
        value = QApplication::applicationFilePath();
        name = "user.org.kde.plasma.file-created-by-executable";
    }
    return {
        .xattrName = name,
        .xattrValue = value.toLocal8Bit()
    };
}

void AnnotatingJob::start()
{
    // 5 minutes is a reasonable amount of time it should take to go from save
    // dialog to filesystem activity. This is mostly here because some applications
    // do further questioning to the user after the file dialog instead of before.
    QTimer::singleShot(300 * 1000, this, &AnnotatingJob::emitResult);

    QtConcurrent::run([=]() {
        QList<QString> failedFiles;
        for (auto path : m_jobs) {
            // Let's see if we can watch for this file being changed...
            auto ok = m_watcher->addPath(path.toLocalFile());

            // If we can't...
            if (!ok) {
                QFileInfo fi(path.toLocalFile());

                // Try watching the folder it's in
                ok = m_watcher->addPath(fi.absoluteDir().absolutePath());
                if (ok) {
                    failedFiles << path.toLocalFile();
                }
            }
        }
        connect(m_watcher, &QFileSystemWatcher::directoryChanged, [=](const QString& path) {
            Q_UNUSED(path)

            auto info = GetXAttrInfo();
            for (auto file : failedFiles) {
                if (GetXAttrInfo().applyToFile(file)) {
                    m_writtenFiles << file;
                    if (m_writtenFiles.count() == m_jobs.count()) {
                        emitResult();
                    }
                }
            }
        });
        connect(m_watcher, &QFileSystemWatcher::fileChanged, [=](const QString& file) {
            GetXAttrInfo().applyToFile(file);
            m_writtenFiles << file;

            if (m_writtenFiles.count() == m_jobs.count()) {
                emitResult();
            }
        });
    });
}