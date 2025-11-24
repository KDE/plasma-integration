// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "kiodelegate.h"

#include <KSandbox>

#include "kioopenwith.h"
#include "kioopenwithxdp.h"

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
