#pragma once

#include <KJob>
#include <QList>
#include <QUrl>
#include <QSet>
#include <QFileSystemWatcher>

// This job watches for the files that a save dialog has selected
// and annotates them with the application that created them
class AnnotatingJob : public KJob {
    Q_OBJECT

public:
    AnnotatingJob(const QList<QUrl>& jobs);
    void start() override;

private:
    QList<QUrl> m_jobs;
    QSet<QString> m_writtenFiles;
    QFileSystemWatcher* m_watcher;
};