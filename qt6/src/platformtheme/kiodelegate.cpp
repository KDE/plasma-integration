// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022-2026 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2020 Kai Uwe Broulik <kde@broulik.de>

#include "kiodelegate.h"

#include <KJob>
#include <KLocalizedString>
#include <KNotification>
#include <KSandbox>

#include "kioopenwith.h"
#include "kioopenwithxdp.h"

using namespace Qt::StringLiterals;

namespace
{
KIO::OpenWithHandlerInterface *makeOpenWithHandlerInterface(QWidget *window)
{
    // Ownership of these transfers to the caller. The caller is expected to eventually pass them into a KJob where the
    // life time will be managed manually (also see KJob::setUiDelegate). We must not set parents here!
    if (static auto sandbox = KSandbox::isInside(); sandbox) {
        return new KIOOpenWithXDP(window, nullptr);
    }
    return new KIOOpenWith(window, nullptr);
}

class MessageDispatch
{
public:
    MessageDispatch() = default;
    virtual ~MessageDispatch() = default;
    Q_DISABLE_COPY_MOVE(MessageDispatch)
    virtual void showErrorMessage(KJob *job) = 0;
    virtual void slotWarning([[maybe_unused]] KJob *job, const QString &message) = 0;
};

class PlasmaMessageDispatch : public MessageDispatch
{
public:
    explicit PlasmaMessageDispatch(const QString &title)
        : m_title(title)
    {
    }

    void showErrorMessage(KJob *job) override
    {
        if (job->error() == KJob::KilledJobError) {
            return;
        }
        auto errorString = [&] {
            if (auto errorString = job->errorString(); !errorString.isEmpty()) {
                return errorString;
            }
            return i18nc("@info", "An unknown error occurred while executing a job: %1", job->error());
        }();
        showNotification(KNotification::Error, errorString);
    }

    void slotWarning([[maybe_unused]] KJob *job, const QString &message) override
    {
        showNotification(KNotification::Notification, message);
    }

private:
    void showNotification(KNotification::StandardEvent standardEvent, const QString &text)
    {
        QString title = [&] {
            if (standardEvent == KNotification::Error && !m_title.isEmpty()) {
                return i18nc("@title of a notification. %1 is the title of a KIO job. Could be pretty much anything.", "%1 (Failed)", m_title);
            }
            return m_title;
        }();
        KNotification::event(standardEvent, title, text);
    }

    QString m_title;
};

template<typename... Args>
std::unique_ptr<MessageDispatch> messageDispatch(Args &&...args)
{
    // We currently lack infrastructure to route messages to a QtQuick UI/window and I could not find a good example scenario where we use a KIO::Delegate from
    // QtQuick. If someone finds an example it'd be good to build out some facilities for this.
    if (qApp->applicationName() == "plasmashell"_L1) {
        return std::make_unique<PlasmaMessageDispatch>(std::forward<Args>(args)...);
    }
    return {};
}

} // namespace

KIOUiDelegate::KIOUiDelegate(KJobUiDelegate::Flags flags, QWidget *window)
    : KIO::JobUiDelegate(flags, window, {makeOpenWithHandlerInterface(window)})
{
}

KJobUiDelegate *KIOUiFactory::createDelegate() const
{
    return new KIOUiDelegate;
}

KJobUiDelegate *KIOUiFactory::createDelegate(KJobUiDelegate::Flags flags, QWidget *window) const
{
    return new KIOUiDelegate(flags, window);
}

bool KIOUiDelegate::setJob(KJob *job)
{
    auto ok = KIO::JobUiDelegate::setJob(job);
    if (ok) {
        connect(job, &KJob::description, this, [this](KJob *, const QString &title, const QPair<QString, QString> &, const QPair<QString, QString> &) {
            m_title = title;
        });
    }
    return ok;
}

void KIOUiDelegate::showErrorMessage()
{
    if (auto dispatch = messageDispatch(m_title); dispatch) {
        dispatch->showErrorMessage(job());
    } else {
        KIO::JobUiDelegate::showErrorMessage();
    }
}

void KIOUiDelegate::slotWarning([[maybe_unused]] KJob *job, const QString &message)
{
    if (auto dispatch = messageDispatch(m_title); dispatch) {
        dispatch->slotWarning(job, message);
    } else {
        KIO::JobUiDelegate::slotWarning(job, message);
    }
}
