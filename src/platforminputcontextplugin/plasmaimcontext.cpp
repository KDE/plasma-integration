/*
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
        ret << TooltipData{upperCase ? item.toUpper() : item, QString::number((i + 1) < 10 ? (i + 1) : 0, 10), i};
        i++;
    }
    return ret;
}

PlasmaIMContext::PlasmaIMContext()
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

bool PlasmaIMContext::isValid() const
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

void PlasmaIMContext::setFocusObject(QObject *object)
{
    m_focusObject = object;
}

void PlasmaIMContext::configChangedHandler(const KConfigGroup &, const QByteArrayList &)
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

    auto isRtl = text[0].character[0].script() == QChar::Script_Arabic || text[0].character[0].script() == QChar::Script_Hebrew;

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

inline bool between(int min, int val, int max)
{
    qDebug() << min << val << max << !(val < min || max < val);
    return !(val < min || max < val);
}

bool PlasmaIMContext::filterEvent(const QEvent *event)
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

            if (!between(Qt::Key_0, ev->key(), Qt::Key_9) && !between(Qt::Key_F1, ev->key(), Qt::Key_F10)) {
                cleanUpState();
                return false;
            }

            auto str = preHoldText;
            bool isUpper = str.isUpper();
            str = str.toLower();

            int keyDataIndex;
            if (ev->key() > Qt::Key_9) {
                keyDataIndex = ev->key() - Qt::Key_F1;
                keyDataIndex++;
            } else {
                keyDataIndex = ev->key() - Qt::Key_0;
                if (keyDataIndex == 0) {
                    keyDataIndex = 10;
                }
            }

            if (KeyData::KeyMappings[str].count() < keyDataIndex) {
                cleanUpState();
                return false;
            }

            auto data = KeyData::KeyMappings[str][keyDataIndex - 1];
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
