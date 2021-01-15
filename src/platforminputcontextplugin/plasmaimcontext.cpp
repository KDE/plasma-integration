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

/// Qt Utilities
#include <QGuiApplication>
#include <QInputMethod>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QQuickWindow>
#include <QStringBuilder>
#include <QToolTip>

// Widgets for the popup
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "data/plasmakeydata.h"

#include "plasmaimcontext.h"

static QList<TooltipData> dataForIndex(const QString &ch, bool upperCase)
{
    QList<TooltipData> ret;
    int i = 0;
    for (auto item : KeyData::KeyMappings[ch]) {
        ret << TooltipData {upperCase ? item.toUpper() : item, QString::number((i + 1) < 10 ? (i + 1) : 0, 10), i};
        i++;
    }
    return ret;
}

PlasmaIMContext::PlasmaIMContext(OptionalContext ctx) : ctx(ctx)
{
    connect(watcher.data(), &KConfigWatcher::configChanged, this, &PlasmaIMContext::configChangedHandler);
}

PlasmaIMContext::~PlasmaIMContext()
{
    if (!popup.isNull()) {
        popup->hide();
        popup->deleteLater();
    }
}

bool PlasmaIMContext::isValidImpl() const
{
    return true;
}

void PlasmaIMContext::cleanUpState()
{
    if (!popup.isNull()) {
        popup->hide();
        popup->deleteLater();
    }

    isPreHold = false;
    preHoldText = QString();
}

void PlasmaIMContext::setFocusObjectImpl(QObject *object)
{
    m_focusObject = object;
}

void PlasmaIMContext::configChangedHandler(const KConfigGroup&, const QByteArrayList&)
{
    config->reparseConfiguration();
}

void PlasmaIMContext::showPopup(const QList<TooltipData> &text)
{
    QPoint position;
    QWindow *parentWin = nullptr;

    auto im = QGuiApplication::inputMethod();
    if (im != nullptr && im->cursorRectangle().isValid()) {
        position = im->cursorRectangle().topRight().toPoint();
        parentWin = QGuiApplication::focusWindow();
    }

    auto isRtl =
        text[0].character[0].script() == QChar::Script_Arabic || text[0].character[0].script() == QChar::Script_Hebrew;

    popup = new QWidget;
    auto grid = new QGridLayout(popup.data());
    popup->setLayoutDirection(isRtl ? Qt::RightToLeft : Qt::LeftToRight);
    popup->setLayout(grid);
    int col = 0;
    for (auto item : text) {
        auto label = new QLabel(item.character, popup.data());
        auto button = new QPushButton(item.number, popup.data());

        button->setMaximumWidth(button->height());

        grid->addWidget(label, 0, col, Qt::AlignCenter);
        grid->addWidget(button, 1, col, Qt::AlignHCenter);

        connect(button, &QPushButton::clicked, [=]() {
            applyReplacement(item.character);
            popup->hide();
            popup->deleteLater();
        });

        col++;
    }

    connect(parentWin, &QWindow::activeChanged, this, &PlasmaIMContext::cleanUpState, Qt::UniqueConnection);

    if (parentWin != nullptr) {
        popup->setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::ToolTip);
        popup->adjustSize();
        popup->move(position + parentWin->framePosition() - QPoint((isRtl ? popup->width() : 0), 0));
        popup->show();
    }
}

void PlasmaIMContext::applyReplacement(const QString &data)
{
    if (m_focusObject != nullptr) {
        QInputMethodEvent ev;
        ev.setCommitString(data, -1, 1);
        QCoreApplication::sendEvent(m_focusObject, &ev);
    }
}

bool PlasmaIMContext::filterEventImpl(const QEvent *event)
{
    bool isAccent = keyboard.readEntry("KeyRepeat", "accent") == QLatin1String("accent");
    bool isNothing = keyboard.readEntry("KeyRepeat", "accent") == QLatin1String("nothing");

    if (!isAccent && !isNothing) {
        return false;
    }

    if (event->type() == QEvent::KeyPress) {
        auto ev = static_cast<const QKeyEvent *>(event);

        // this is the state when we have a held key
        if (isPreHold) {
            if (ev->isAutoRepeat() && ev->text() == preHoldText)
                return true;

            if (ev->key() < 0x30 || 0x39 < ev->key()) {
                cleanUpState();
                return false;
            }

            auto str = preHoldText;
            bool isUpper = str.isUpper();
            str = str.toLower();

            int key = ev->key() - 0x30;
            if (key == 0) {
                key = 10;
            }
            if (KeyData::KeyMappings[str].count() < key) {
                cleanUpState();
                return false;
            }

            auto data = KeyData::KeyMappings[str][key - 1];
            applyReplacement(isUpper ? data.toUpper() : data);
            isPreHold = false;
            preHoldText = QString();
            popup->hide();
            return true;
        }

        // this is the state before we have a held key
        if (ev->isAutoRepeat()) {
            if (isNothing)
                return true;

            if (!isPreHold) {
                if (ev->text().isEmpty())
                    return false;
                if (!KeyData::KeyMappings.contains(ev->text().toLower()))
                    return false;

                auto tooltipText = dataForIndex(ev->text().toLower(), ev->text().isUpper());
                showPopup(tooltipText);

                isPreHold = true;
                preHoldText = ev->text();
            }

            return true;
        }

        cleanUpState();
    }

    return false;
}
