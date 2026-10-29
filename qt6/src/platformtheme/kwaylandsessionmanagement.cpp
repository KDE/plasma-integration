/*
    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwaylandsessionmanagement.h"

#include <QGuiApplication>

#if QT_VERSION >= QT_VERSION_CHECK(6, 12, 0) && !defined(QT_NO_SESSIONMANAGER)
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QLockFile>
#include <QPlatformSurfaceEvent>
#include <QSessionManager>
#include <QStandardPaths>
#include <QTimer>
#include <QWindow>
#include <private/qguiapplication_p.h>
#include <private/qsessionmanager_p.h>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformsessionmanager.h>

#include <KConfigGroup>
#include <KSharedConfig>

#include <memory>

namespace
{
class NativeSessionPersistence : public QObject
{
public:
    explicit NativeSessionPersistence(QObject *parent)
        : QObject(parent)
    {
        qApp->installEventFilter(this);
        m_lockRetryTimer.setInterval(5000);
        connect(&m_lockRetryTimer, &QTimer::timeout, this, [this] {
            // Existing windows keep their native session. A new lock owner
            // persists that session instead of restoring another backend.
            saveSessionId();
        });
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
            saveSessionId();
        });
    }

protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (!m_initialized && event->type() == QEvent::PlatformSurface) {
            auto *window = qobject_cast<QWindow *>(object);
            auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
            if (window && !window->parent() && !window->flags().testFlag(Qt::ForeignWindow) && window->type() != Qt::Popup
                && window->type() != Qt::ToolTip && surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
                // Qt Wayland creates the wl_surface in the platform window
                // constructor, but defers the xdg_toplevel until setVisible().
                initialize();
            }
        }
        return false;
    }

private:
    void initialize()
    {
        auto *application = QGuiApplicationPrivate::instance();
        if (!application->session_manager) {
            return;
        }
        // Explicit desktop-session restores and application opt-outs belong to Qt.
        if (qApp->testAttribute(Qt::AA_DisableSessionManager) || qApp->isSessionRestored()) {
            m_initialized = true;
            return;
        }
        // Qt 6.12 obtains the ID synchronously. Keep initialization retryable
        // if no native session is available at this surface event.
        if (qApp->sessionId().isEmpty()) {
            return;
        }

        const QString appId = QGuiApplication::desktopFileName().isEmpty() ? QCoreApplication::applicationName() : QGuiApplication::desktopFileName();
        if (appId.isEmpty()) {
            return;
        }

        m_config = KSharedConfig::openConfig();
        m_appId = appId;
        const QString runtimePath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
        if (runtimePath.isEmpty()) {
            return;
        }
        QString configPath = m_config->name();
        if (QDir::isRelativePath(configPath)) {
            configPath = QDir(QStandardPaths::writableLocation(m_config->locationType())).absoluteFilePath(configPath);
        }
        const QFileInfo configFile(configPath);
        QString canonicalPath = configFile.canonicalFilePath();
        if (canonicalPath.isEmpty()) {
            // The config file need not exist on the application's first run.
            const QString directory = configFile.dir().canonicalPath();
            canonicalPath = directory.isEmpty() ? configFile.absoluteFilePath() : QDir(directory).filePath(configFile.fileName());
        }
        const QByteArray identity = (QDir::cleanPath(canonicalPath) + QLatin1Char('/') + appId).toUtf8();
        const QString lockName = QString::fromLatin1(QCryptographicHash::hash(identity, QCryptographicHash::Sha256).toHex());
        m_lock = std::make_unique<QLockFile>(runtimePath + QLatin1String("/plasma-qt-session-") + lockName + QLatin1String(".lock"));
        m_initialized = true;
        // A second live instance must not replace the first instance's session.
        // Retry storage ownership, but never replace a backend after this point.
        if (!m_lock->tryLock(0)) {
            m_lockRetryTimer.start();
            return;
        }
        m_storeSession = true;

        const QString savedId = KConfigGroup(m_config, QStringLiteral("XdgSessionManagement")).readEntry(appId, QString());
        if (!savedId.isEmpty() && savedId != qApp->sessionId()) {
            auto *manager = static_cast<QSessionManagerPrivate *>(QObjectPrivate::get(application->session_manager));
            auto *previous = manager->platformSessionManager;
            // Qt has no public setter for the initial session ID. Recreate only
            // its native backend through QPA, before the first xdg_toplevel is
            // made. Qt still owns every protocol object and window association.
            manager->platformSessionManager = application->platformIntegration()->createPlatformSessionManager(savedId, qApp->sessionKey());
            delete previous;
        }
        saveSessionId();
    }

    void saveSessionId()
    {
        if (!m_lock || !QGuiApplicationPrivate::instance()->session_manager) {
            return;
        }
        const QString id = qApp->sessionId();
        if (id.isEmpty()) {
            return;
        }
        if (!m_storeSession) {
            if (!m_lock->tryLock(0)) {
                return;
            }
            m_storeSession = true;
            m_lockRetryTimer.stop();
        }
        // QSessionManager::sessionId() delegates to platformSessionManager, so
        // this is the current native ID even after replacing that backend.
        KConfigGroup group(m_config, QStringLiteral("XdgSessionManagement"));
        if (group.readEntry(m_appId, QString()) == id) {
            return;
        }
        group.writeEntry(m_appId, id);
        group.sync();
    }

    bool m_initialized = false;
    bool m_storeSession = false;
    KSharedConfig::Ptr m_config;
    QString m_appId;
    std::unique_ptr<QLockFile> m_lock;
    QTimer m_lockRetryTimer;
};
}
#endif

void initializeWaylandSessionManagement()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 12, 0) && !defined(QT_NO_SESSIONMANAGER)
    constexpr auto property = "org.kde.plasma.integration.nativeSessionPersistence";
    if (!qApp->property(property).toBool()) {
        qApp->setProperty(property, true);
        new NativeSessionPersistence(qApp);
    }
#endif
}
