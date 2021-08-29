/*
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMAIMCONTEXT_H
#define PLASMAIMCONTEXT_H

#include <QPointer>
#include <QWidget>

#include <KConfigGroup>
#include <KConfigWatcher>
#include <KSharedConfig>

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
    bool filterEvent(const QEvent *event) Q_DECL_OVERRIDE;

private:
    void cleanUpState();
    void applyReplacement(const QString &data);
    void showPopup(const QList<TooltipData> &text);
    void configChangedHandler(const KConfigGroup &grp, const QByteArrayList &names);

    QPointer<QWidget> popup;
    QPointer<QObject> m_focusObject = nullptr;

    bool isPreHold = false;
    QString preHoldText = QString();
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("kcminputrc"));
    KConfigGroup keyboard = KConfigGroup(config, "Keyboard");
    KConfigWatcher::Ptr watcher = KConfigWatcher::create(config);
};

QT_END_NAMESPACE

#endif
