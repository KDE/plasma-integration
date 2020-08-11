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

#include <KConfigGroup>
#include <KSharedConfig>
#include <KConfigWatcher>

#include <qpa/qplatforminputcontext.h>

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
    PlasmaIMContext();
    ~PlasmaIMContext();

    bool isValid() const Q_DECL_OVERRIDE;
    void setFocusObject(QObject *object) Q_DECL_OVERRIDE;
    bool filterEvent(const QEvent* event) Q_DECL_OVERRIDE;

private:

    void cleanUpState();
    void applyReplacement(const QString& data);
    void showPopup(const QList<TooltipData>& text);
    void configChangedHandler(const KConfigGroup& grp, const QByteArrayList& names);

    QPointer<QWidget> popup;
    QPointer<QObject> m_focusObject = nullptr;

    bool isPreHold = false;
    QString preHoldText = QString();
    KSharedConfig::Ptr config = KSharedConfig::openConfig( QStringLiteral("kcminputrc") );
    KConfigGroup keyboard = KConfigGroup(config, "Keyboard");
    KConfigWatcher::Ptr watcher = KConfigWatcher::create(config);

};

QT_END_NAMESPACE

#endif
