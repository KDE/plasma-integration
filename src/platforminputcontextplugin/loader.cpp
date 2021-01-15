#include <QDebug>
#include <qpa/qplatforminputcontextfactory_p.h>

#include "loader.h"

OptionalContext chainloadContext() {
    QByteArray env = qgetenv("PLASMA_IM_MODULE");
    if (env.isNull()) {
        return {};
    }

    auto ctx = QPlatformInputContextFactory::create(QString::fromLocal8Bit(env));
    if (!ctx) {
        return {};
    }

    return {QSharedPointer<QPlatformInputContext>(ctx)};
}
