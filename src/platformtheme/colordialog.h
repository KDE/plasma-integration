/*  This file is part of the KDE libraries
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

#ifndef COLOR_DIALOG_H
#define COLOR_DIALOG_H

#include <QDialog>
#include <QQuickWidget>
#include <QPointer>
#include <QQuickPaintedItem>
#include <QJsonArray>
#include <KSharedConfig>
#include <KConfigWatcher>
#include <qpa/qplatformdialoghelper.h>

class ColorDialogHelper;

class ColorDialog : public QDialog
{
    Q_OBJECT

public:

    QPointer<QQuickWidget> view;
    explicit ColorDialog(ColorDialogHelper* parent);
};

class ColorDialogHelper : public QPlatformColorDialogHelper
{
    Q_OBJECT

    QPointer<ColorDialog> dialog;
    void prepareDialog();

    QJsonArray _savedColors;
    Q_PROPERTY(QJsonArray savedColors READ savedColors WRITE setSavedColors NOTIFY savedColorsChanged)

    KSharedConfigPtr _savedColorsConfig;
    QSharedPointer<KConfigWatcher> _watcher;

public:
    ColorDialogHelper();
    ~ColorDialogHelper();
    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parentWindow) override;
    void hide() override;
    void setCurrentColor(const QColor& color) override;
    QColor currentColor() const override;

    QVariant styleHint(StyleHint hint) const override;

    Q_INVOKABLE void pick();
    Q_INVOKABLE void changed(QColor c) { Q_EMIT currentColorChanged(c); }

    QJsonArray savedColors() const;
    void setSavedColors(QJsonArray);

    Q_SIGNAL void savedColorsChanged();
};

class HSVCircle : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(qreal value MEMBER value NOTIFY valueChanged)

public:
    HSVCircle(QQuickItem* parent = nullptr);

    qreal value;
    Q_SIGNAL void valueChanged();
    void paint(QPainter *painter) override;
    Q_INVOKABLE QColor mapToRGB(int x, int y) const;
    Q_INVOKABLE QPointF mapFromRGB(const QColor& in) const;
};

#endif