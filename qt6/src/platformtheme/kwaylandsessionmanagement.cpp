/*
    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kwaylandsessionmanagement.h"

#include <QGuiApplication>

#if QT_VERSION >= QT_VERSION_CHECK(6, 12, 0) && !defined(QT_NO_SESSIONMANAGER)
#include <QCryptographicHash>
#include <QLockFile>
#include <QPlatformSurfaceEvent>
#include <QSessionManager>
#include <QStandardPaths>
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
        m_initialized = true;

        // Explicit desktop-session restores and application opt-outs belong to Qt.
        // An empty native ID also means that no compositor session was created.
        if (qApp->testAttribute(Qt::AA_DisableSessionManager) || qApp->isSessionRestored() || qApp->sessionId().isEmpty()) {
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
        const QByteArray identity = (m_config->name() + QLatin1Char('/') + appId).toUtf8();
        const QString lockName = QString::fromLatin1(QCryptographicHash::hash(identity, QCryptographicHash::Sha256).toHex());
        m_lock = std::make_unique<QLockFile>(runtimePath + QLatin1String("/plasma-qt-session-") + lockName + QLatin1String(".lock"));
        // A second live instance must not replace the first instance's session.
        if (!m_lock->tryLock(0)) {
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
        if (!m_storeSession || !QGuiApplicationPrivate::instance()->session_manager) {
            return;
        }
        const QString id = qApp->sessionId();
        if (id.isEmpty()) {
            return;
        }
        KConfigGroup group(m_config, QStringLiteral("XdgSessionManagement"));
        group.writeEntry(m_appId, id);
        group.sync();
    }

    bool m_initialized = false;
    bool m_storeSession = false;
    KSharedConfig::Ptr m_config;
    QString m_appId;
    std::unique_ptr<QLockFile> m_lock;
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
