// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022-2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QWidget>
#include <private/qdesktopunixservices_p.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <KJob>
#include <KJobWidgets>
#include <KJobWindows>

namespace WindowId
{
struct MakeResult {
    QString portalIdentifier;
    QWidget *widget = nullptr;
};

inline MakeResult make(KJob *job, QWidget *parentWidget)
{
    // We resolve the widget because part of our code uses KMessageBox. It'd like a parent widget.
    auto widget = [&]() -> QWidget * {
        if (job) {
            if (auto widget = KJobWidgets::window(job); widget) {
                return widget;
            }
        }

        return parentWidget;
    }();

    // Also resolve a window for exporting.
    auto window = [&]() -> QWindow * {
        if (widget) {
            widget->window()->winId(); // ensure we have a handle so we can export a window (without this windowHandle() may be null)
            return widget->windowHandle();
        }

        if (job) {
            return KJobWindows::window(job);
        }

        return nullptr;
    }();

    if (window) {
        auto services = QGuiApplicationPrivate::platformIntegration()->services();
        if (auto unixServices = dynamic_cast<QDesktopUnixServices *>(services)) {
            return {.portalIdentifier = unixServices->portalWindowIdentifier(window), .widget = widget};
        }
    }

    return {};
}
} // namespace WindowId
