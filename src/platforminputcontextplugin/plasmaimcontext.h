/*
 *  Copyright 2020 Carson Black <uhhadd@gmail.com>
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

#ifndef PLASMAIMCONTEXT_H
#define PLASMAIMCONTEXT_H

#include <QPointer>
#include <QWidget>
#include <QLocale>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KConfigWatcher>

#include <qpa/qplatforminputcontext.h>

#include "loader.h"

QT_BEGIN_NAMESPACE

struct TooltipData {
    QString character;
    QString number;
    int idx;
};

class PlasmaIMContext : public QPlatformInputContext
{
    Q_OBJECT

public:
    PlasmaIMContext(OptionalContext ctx = OptionalContext());
    ~PlasmaIMContext();

    bool isValidImpl() const;
    void setFocusObjectImpl(QObject *object);
    bool filterEventImpl(const QEvent* event);

private:

    void cleanUpState();
    void applyReplacement(const QString& data);
    void showPopup(const QList<TooltipData>& text);
    void configChangedHandler(const KConfigGroup& grp, const QByteArrayList& names);

    QPointer<QWidget> popup;
    QPointer<QObject> m_focusObject = nullptr;
    OptionalContext ctx;

    bool isPreHold = false;
    QString preHoldText = QString();
    KSharedConfig::Ptr config = KSharedConfig::openConfig( QStringLiteral("kcminputrc") );
    KConfigGroup keyboard = KConfigGroup(config, "Keyboard");
    KConfigWatcher::Ptr watcher = KConfigWatcher::create(config);

    bool isValid() const override
    {
        if (ctx) {
            return isValidImpl() || ctx.value()->isValid();
        }
        return isValidImpl();
    }

    bool hasCapability(Capability capability) const override
    {
        if (ctx) {
            return ctx.value()->hasCapability(capability);
        }
        return QPlatformInputContext::hasCapability(capability);
    }


    void reset() override
    {
        if (ctx) {
            ctx.value()->reset();
            return;
        }
        QPlatformInputContext::reset();
    }

    void commit() override
    {
        if (ctx) {
            ctx.value()->commit();
            return;
        }
        QPlatformInputContext::commit();
    }

    void update(Qt::InputMethodQueries q) override
    {
        if (ctx) {
            ctx.value()->update(q);
            return;
        }
        QPlatformInputContext::update(q);
    }

    void invokeAction(QInputMethod::Action a, int cursorPosition) override
    {
        if (ctx) {
            ctx.value()->invokeAction(a, cursorPosition);
            return;
        }
        QPlatformInputContext::invokeAction(a, cursorPosition);
    }

    bool filterEvent(const QEvent *event) override
    {
        if (ctx) {
            return filterEventImpl(event) || ctx.value()->filterEvent(event);
        }
        return filterEventImpl(event);
    }

    QRectF keyboardRect() const override
    {
        if (ctx) {
            return ctx.value()->keyboardRect();
        }
        return QPlatformInputContext::keyboardRect();
    }

    bool isAnimating() const override
    {
        if (ctx) {
            return ctx.value()->isAnimating();
        }
        return QPlatformInputContext::isAnimating();
    }

    void showInputPanel() override
    {
        if (ctx) {
            return ctx.value()->showInputPanel();
        }
        QPlatformInputContext::showInputPanel();
    }

    void hideInputPanel() override
    {
        if (ctx) {
            ctx.value()->hideInputPanel();
            return;
        }
        return QPlatformInputContext::hideInputPanel();
    }

    bool isInputPanelVisible() const override
    {
        if (ctx) {
            return ctx.value()->isInputPanelVisible();
        }
        return QPlatformInputContext::isInputPanelVisible();
    }

    QLocale locale() const override
    {
        if (ctx) {
            return ctx.value()->locale();
        }
        return QPlatformInputContext::locale();
    }

    Qt::LayoutDirection inputDirection() const override
    {
        if (ctx) {
            return ctx.value()->inputDirection();
        }
        return QPlatformInputContext::inputDirection();
    }

    void setFocusObject(QObject *object) override
    {
        setFocusObjectImpl(object);
        if (ctx) {
            ctx.value()->setFocusObject(object);
            return;
        }
    }

};

QT_END_NAMESPACE

#endif
