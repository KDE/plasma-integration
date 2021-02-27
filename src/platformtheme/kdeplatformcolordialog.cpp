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

#include "kdeplatformcolordialog.h"
#include <KLocalizedContext>
#include <KSharedConfig>
#include <QClipboard>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QStackedLayout>
#include <QtMath>

ColorDialog::ColorDialog(ColorDialogHelper *parent)
    : QDialog()
{
    setLayout(new QStackedLayout);

    qmlRegisterType<HSVCircle>("org.kde.private.plasmaintegration", 1, 0, "HSVCircle");
    qmlRegisterType<PencilTip>("org.kde.private.plasmaintegration", 1, 0, "PencilTip");

    view = new QQuickWidget(this);

    view->rootContext()->setContextObject(new KLocalizedContext(view.data()));
    view->rootContext()->setContextProperty(QStringLiteral("helper"), parent);

    const QUrl dialogURL(QStringLiteral("qrc:/org/kde/plasma/integration/ColorDialog.qml"));
    QObject::connect(view, &QQuickWidget::statusChanged, [=](QQuickWidget::Status status) {
        if (status == QQuickWidget::Error) {
            qDebug() << view->errors();
            qFatal("Failed to load color dialog.");
        }
    });
    view->setSource(dialogURL);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    layout()->addWidget(view);
    layout()->setContentsMargins(0, 0, 0, 0);

    setMinimumSize(400, 500);
    setBaseSize(400, 500);
}

ColorDialogHelper::ColorDialogHelper()
    : QPlatformColorDialogHelper()
{
    m_savedColorsConfig = KSharedConfig::openConfig(QStringLiteral("plasmacolorpicker"));
    m_watcher = KConfigWatcher::create(m_savedColorsConfig);
    auto setColors = [this] {
        auto json = m_savedColorsConfig->group("ColorPicker").readEntry("SavedColors", QStringLiteral("[]"));
        m_savedColors = QJsonDocument::fromJson(json.toLocal8Bit()).array();
        Q_EMIT savedColorsChanged();
    };
    connect(m_watcher.data(), &KConfigWatcher::configChanged, [=] {
        m_savedColorsConfig->reparseConfiguration();
        setColors();
    });
    setColors();
}

ColorDialogHelper::~ColorDialogHelper()
{
}

void ColorDialogHelper::exec()
{
    m_dialog->exec();
    Q_EMIT colorSelected(currentColor());
    Q_EMIT accept();
}

void ColorDialogHelper::prepareDialog()
{
    m_dialog = new ColorDialog(this);

    QObject::connect(m_dialog, &QDialog::finished, this, [=] {
        Q_EMIT colorSelected(currentColor());
    });
}

bool ColorDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality modality, QWindow *parentWindow)
{
    if (m_dialog.isNull()) {
        prepareDialog();
    }

    m_dialog->setWindowModality(modality);
    m_dialog->setWindowFlags(windowFlags);

    m_dialog->winId();
    m_dialog->windowHandle()->setTransientParent(parentWindow);

    m_dialog->show();
    return true;
}

void ColorDialogHelper::hide()
{
    m_dialog->hide();
}

void ColorDialogHelper::pick()
{
    auto msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                              QStringLiteral("/ColorPicker"),
                                              QStringLiteral("org.kde.kwin.ColorPicker"),
                                              QStringLiteral("pick"));

    auto call = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        if (!watcher->isError()) {
            unsigned int val;

            // we need this tomfoolery because kwin replies with a '(u)' instead
            // of an 'u'.
            //
            // more tomfoolery: non-const beginStructure() overload
            // will cause a crash here. the 'arg' *must* be const.
            const auto arg = watcher->reply().arguments()[0].value<QDBusArgument>();
            arg.beginStructure();
            arg >> val;
            arg.endStructure();

            setCurrentColor(QColor::fromRgba(val));
        } else {
            qWarning() << watcher->error();
        }
        watcher->deleteLater();
    });
}

void ColorDialogHelper::setCurrentColor(const QColor &color)
{
    if (m_dialog.isNull()) {
        return;
    }

    m_dialog->view->rootObject()->setProperty("currentColor", color);
}

QVariant ColorDialogHelper::styleHint(StyleHint hint) const
{
    if (hint == DialogIsQtWindow) {
        return true;
    }

    return QPlatformDialogHelper::styleHint(hint);
}

QColor ColorDialogHelper::currentColor() const
{
    return m_dialog->view->rootObject()->property("currentColor").value<QColor>();
}

QColor HSVCircle::mapToRGB(int x, int y) const
{
    Q_ASSERT(width() == height());
    const auto size = int(width());
    const auto radius = size / 2;

    const auto h = (qAtan2(x - radius, y - radius) + M_PI) / (2.0 * M_PI);
    const auto s = qSqrt(qPow(x - radius, 2) + qPow(y - radius, 2)) / radius;
    const auto v = value;

    return QColor::fromHsvF(qBound(0.0, h, 1.0), qBound(0.0, s, 1.0), qBound(0.0, v, 1.0));
}

HSVCircle::HSVCircle(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    connect(this, &HSVCircle::valueChanged, [this] {
        update();
    });
}

// TODO: figure out fancy math stuff instead of brute force
// h is just angle and s is just distance from center,
// but i can't figure how to use that information to
// do something useful
QPointF HSVCircle::mapFromRGB(const QColor &in) const
{
    Q_ASSERT(width() == height());
    const auto size = width();

    const auto h1 = in.hueF(), s1 = in.saturationF(), v1 = in.valueF();

    qreal distance = 500.0;
    QPointF closest;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            const auto kule = mapToRGB(x, y);
            const auto h2 = kule.hueF(), s2 = kule.saturationF(), v2 = kule.valueF();

            const auto thisDist = qPow(qSin(h1) * s1 * v1 - qSin(h2) * s2 * v2, 2) + qPow(qCos(h1) * s1 * v1 - qCos(h2) * s2 * v2, 2) + qPow(v1 - v2, 2);

            if (distance > thisDist) {
                distance = thisDist;
                closest = QPointF(x, y);
            }
        }
    }

    return closest;
}

void HSVCircle::paint(QPainter *painter)
{
    Q_ASSERT(width() == height());
    const auto size = int(width());

    QRgb *pixels = new QRgb[size * size];

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            pixels[x + y * size] = mapToRGB(x, y).rgb();
        }
    }

    painter->drawImage(0, 0, QImage((uchar *)pixels, width(), height(), QImage::Format_ARGB32));
    delete[] pixels;
}

QJsonArray ColorDialogHelper::savedColors() const
{
    return m_savedColors;
}

void ColorDialogHelper::setSavedColors(const QJsonArray &obj)
{
    m_savedColors = obj;
    Q_EMIT savedColorsChanged();

    QJsonDocument doc(obj);

    m_savedColorsConfig->group("ColorPicker").writeEntry("SavedColors", QString::fromLocal8Bit(doc.toJson()), KConfig::Notify);
    m_savedColorsConfig->sync();
}

#define qGuiClipboard QGuiApplication::clipboard()

void ColorDialogHelper::copy()
{
    const auto color = currentColor();

    auto mimedata = new QMimeData;
    mimedata->setColorData(color);
    mimedata->setText(color.name());

    qGuiClipboard->setMimeData(mimedata);
}

bool ColorDialogHelper::paste()
{
    const auto mimeData = qGuiClipboard->mimeData();
    if (mimeData->hasColor()) {
        setCurrentColor(mimeData->colorData().value<QColor>());
        return true;
    }

    const auto text = qGuiClipboard->text();
    const auto color = QColor(text);

    if (color.isValid()) {
        setCurrentColor(color);
        return true;
    }

    return false;
}

PencilTip::PencilTip(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    connect(this, &PencilTip::colorChanged, [this] {
        update();
    });
}

void PencilTip::paint(QPainter *painter)
{
    painter->setPen(Qt::transparent);
    painter->setRenderHint(QPainter::Antialiasing);

    QLinearGradient grad(0, 0, width(), 0);
    grad.setStops({
        {0.0 / 100.0, QColor("#9c725a")},
        {65.0 / 100.0, QColor("#F1C6A4")},
        {100.0 / 100.0, QColor("#E2BF95")},
    });

    QLinearGradient tipGrad(0, 0, width(), 0);
    tipGrad.setStops({
        {0.0 / 100.0, Qt::transparent},
        {45.0 / 100.0, QColor::fromRgbF(255, 255, 255, 0.3)},
        {100.0 / 100.0, QColor::fromRgbF(0, 0, 0, 0.1)},
    });

    QPainterPath path;
    path.moveTo(0, height());
    path.lineTo(width() / 2, 0);
    path.lineTo(width(), height());
    path.lineTo(0, height());

    painter->setBrush(grad);
    painter->drawPath(path);

    const int tipHeight = 10;

    painter->setClipPath(path);
    painter->fillRect(0, 0, width(), tipHeight, color);
    painter->fillRect(0, 0, width(), tipHeight, tipGrad);
}