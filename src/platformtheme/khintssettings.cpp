/*  This file is part of the KDE libraries
 *  Copyright 2013 Kevin Ottens <ervin+bluesystems@kde.org>
 *  Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  Copyright 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
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

#include "khintssettings.h"

#include <QDebug>
#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QToolBar>
#include <QPalette>
#include <QToolButton>
#include <QMainWindow>
#include <QApplication>
#include <QGuiApplication>
#include <QDialogButtonBox>
#include <QScreen>

#include <QDBusConnection>
#include <QDBusInterface>

#include <kiconloader.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kcolorscheme.h>

#ifndef UNIT_TEST
#include <config-platformtheme.h>
#else
#define HAVE_X11 0
#endif
#if HAVE_X11
#include <QX11Info>
#include <X11/Xcursor/Xcursor.h>
#endif

static const QString defaultLookAndFeelPackage = QStringLiteral("org.kde.breeze.desktop");

KHintsSettings::KHintsSettings() : QObject(0)
{
    mKdeGlobals = KSharedConfig::openConfig(QStringLiteral("kdeglobals"), KConfig::NoGlobals);
    KConfigGroup cg(mKdeGlobals, "KDE");

    // try to extract the proper defaults file from a lookandfeel package
    const QString looknfeel = cg.readEntry("LookAndFeelPackage", defaultLookAndFeelPackage);
    mDefaultLnfConfig = KSharedConfig::openConfig(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/look-and-feel/" + looknfeel + "/contents/defaults"));
    if (looknfeel != defaultLookAndFeelPackage) {
        mLnfConfig = KSharedConfig::openConfig(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/look-and-feel/" + defaultLookAndFeelPackage + "/contents/defaults"));
    }


    m_hints[QPlatformTheme::CursorFlashTime] = qBound(200, cg.readEntry("CursorBlinkRate", 1000), 2000);
    m_hints[QPlatformTheme::MouseDoubleClickInterval] = cg.readEntry("DoubleClickInterval", 400);
    m_hints[QPlatformTheme::StartDragDistance] = cg.readEntry("StartDragDist", 10);
    m_hints[QPlatformTheme::StartDragTime] = cg.readEntry("StartDragTime", 500);

    KConfigGroup cgToolbar(mKdeGlobals, "Toolbar style");
    m_hints[QPlatformTheme::ToolButtonStyle] = toolButtonStyle(cgToolbar);

    KConfigGroup cgToolbarIcon(mKdeGlobals, "MainToolbarIcons");
    m_hints[QPlatformTheme::ToolBarIconSize] = cgToolbarIcon.readEntry("Size", 22);

    m_hints[QPlatformTheme::ItemViewActivateItemOnSingleClick] = cg.readEntry("SingleClick", true);

    m_hints[QPlatformTheme::SystemIconThemeName] = readConfigValue(QStringLiteral("Icons"), QStringLiteral("Theme"), "breeze");

    m_hints[QPlatformTheme::SystemIconFallbackThemeName] = "hicolor";
    m_hints[QPlatformTheme::IconThemeSearchPaths] = xdgIconThemePaths();

    QStringList styleNames;
    styleNames << cg.readEntry("widgetStyle", QString())
               << QStringLiteral("breeze")
               << QStringLiteral("oxygen")
               << QStringLiteral("fusion")
               << QStringLiteral("windows");
    const QString lnfStyle = readConfigValue(QStringLiteral("KDE"), QStringLiteral("widgetStyle"), QString()).toString();
    if (!lnfStyle.isEmpty()) {
        styleNames.removeOne(lnfStyle);
        styleNames.prepend(lnfStyle);
    }
    m_hints[QPlatformTheme::StyleNames] = styleNames;

    m_hints[QPlatformTheme::DialogButtonBoxLayout] = QDialogButtonBox::KdeLayout;
    m_hints[QPlatformTheme::DialogButtonBoxButtonsHaveIcons] = cg.readEntry("ShowIconsOnPushButtons", true);
    m_hints[QPlatformTheme::UseFullScreenForPopupMenu] = true;
    m_hints[QPlatformTheme::KeyboardScheme] = QPlatformTheme::KdeKeyboardScheme;
    m_hints[QPlatformTheme::UiEffects] = cg.readEntry("GraphicEffectsLevel", 0) != 0 ? QPlatformTheme::GeneralUiEffect : 0;
    m_hints[QPlatformTheme::IconPixmapSizes] = QVariant::fromValue(QList<int>() << 512 << 256 << 128 << 64 << 32 << 22 << 16 << 8);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    m_hints[QPlatformTheme::WheelScrollLines] = cg.readEntry("WheelScrollLines", 3);
#endif
    if (qobject_cast<QApplication *>(QCoreApplication::instance())) {
        QApplication::setWheelScrollLines(cg.readEntry("WheelScrollLines", 3));
    }

    bool showIcons = cg.readEntry("ShowIconsInMenuItems", !QApplication::testAttribute(Qt::AA_DontShowIconsInMenus));
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !showIcons);

    QMetaObject::invokeMethod(this, "delayedDBusConnects", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "setupIconLoader", Qt::QueuedConnection);

    loadPalettes();
}

KHintsSettings::~KHintsSettings()
{
    qDeleteAll(m_palettes);
}

QVariant KHintsSettings::readConfigValue(const QString &group, const QString &key, const QVariant &defaultValue)
{
    KConfigGroup userCg(mKdeGlobals, group);
    QVariant value = userCg.readEntry(key, QString());

    if (!value.isNull()) {
        return value;
    }

    if (mLnfConfig) {
        KConfigGroup lnfCg(mLnfConfig, "kdeglobals");
        lnfCg = KConfigGroup(&lnfCg, group);
        if (lnfCg.isValid()) {
            value = lnfCg.readEntry(key, defaultValue);

            if (!value.isNull()) {
                return value;
            }
        }
    }

    KConfigGroup lnfCg(mDefaultLnfConfig, group);
    if (lnfCg.isValid()) {
        return lnfCg.readEntry(key, defaultValue);
    }

    return defaultValue;
}

QStringList KHintsSettings::xdgIconThemePaths() const
{
    QStringList paths;

    const QFileInfo homeIconDir(QDir::homePath() + QStringLiteral("/.icons"));
    if (homeIconDir.isDir()) {
        paths << homeIconDir.absoluteFilePath();
    }

    QString xdgDirString = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));

    if (xdgDirString.isEmpty()) {
        xdgDirString = QStringLiteral("/usr/local/share:/usr/share");
    }

    foreach (const QString &xdgDir, xdgDirString.split(QLatin1Char(':'))) {
        const QFileInfo xdgIconsDir(xdgDir + QStringLiteral("/icons"));
        if (xdgIconsDir.isDir()) {
            paths << xdgIconsDir.absoluteFilePath();
        }
    }

    return paths;
}

void KHintsSettings::delayedDBusConnects()
{
    QDBusConnection::sessionBus().connect(QString(), QStringLiteral("/KToolBar"), QStringLiteral("org.kde.KToolBar"),
                                          QStringLiteral("styleChanged"), this, SLOT(toolbarStyleChanged()));
    QDBusConnection::sessionBus().connect(QString(), QStringLiteral("/KGlobalSettings"), QStringLiteral("org.kde.KGlobalSettings"),
                                          QStringLiteral("notifyChange"), this, SLOT(slotNotifyChange(int,int)));
}

void KHintsSettings::setupIconLoader()
{
    connect(KIconLoader::global(), &KIconLoader::iconChanged, this, &KHintsSettings::iconChanged);
}

void KHintsSettings::toolbarStyleChanged()
{
    mKdeGlobals->reparseConfiguration();
    KConfigGroup cg(mKdeGlobals, "Toolbar style");

    m_hints[QPlatformTheme::ToolButtonStyle] = toolButtonStyle(cg);
    //from gtksymbol.cpp
    QWidgetList widgets = QApplication::allWidgets();
    for (int i = 0; i < widgets.size(); ++i) {
        QWidget *widget = widgets.at(i);
        if (qobject_cast<QToolButton *>(widget)) {
            QEvent event(QEvent::StyleChange);
            QApplication::sendEvent(widget, &event);
        }
    }
}

void KHintsSettings::slotNotifyChange(int type, int arg)
{
    mKdeGlobals->reparseConfiguration();
    KConfigGroup cg(mKdeGlobals, "KDE");

    switch (type) {
    case PaletteChanged: {
        loadPalettes();

        //QApplication::setPalette and QGuiApplication::setPalette are different functions
        //and non virtual. Call the correct one
        if (qobject_cast<QApplication *>(QCoreApplication::instance())) {
            QApplication::setPalette(*m_palettes[QPlatformTheme::SystemPalette]);
        } else if (qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
            QGuiApplication::setPalette(*m_palettes[QPlatformTheme::SystemPalette]);
        }
        break;
    }
    case SettingsChanged: {

        SettingsCategory category = static_cast<SettingsCategory>(arg);
        if (category == SETTINGS_QT || category == SETTINGS_MOUSE) {
            updateQtSettings(cg);
        } else if (category == SETTINGS_STYLE) {
            m_hints[QPlatformTheme::DialogButtonBoxButtonsHaveIcons] = cg.readEntry("ShowIconsOnPushButtons", true);
            m_hints[QPlatformTheme::UiEffects] = cg.readEntry("GraphicEffectsLevel", 0) != 0 ? QPlatformTheme::GeneralUiEffect : 0;
        }
        break;
    }
    case ToolbarStyleChanged: {
        toolbarStyleChanged();
        break;
    }
    case IconChanged:
        iconChanged(arg); //Once the KCM is ported to use IconChanged, this should not be needed
        break;
    case CursorChanged:
        updateCursorTheme();
        break;
    case StyleChanged: {
        QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
        if (!app) {
            return;
        }

        const QString theme = cg.readEntry("widgetStyle", QString());
        if (theme.isEmpty()) {
            return;
        }

        QStringList styleNames;
        styleNames << cg.readEntry("widgetStyle", QString())
                << QStringLiteral("breeze")
                << QStringLiteral("oxygen")
                << QStringLiteral("fusion")
                << QStringLiteral("windows");
        const QString lnfStyle = readConfigValue(QStringLiteral("KDE"), QStringLiteral("widgetStyle"), QString()).toString();
        if (!lnfStyle.isEmpty() && !styleNames.contains(lnfStyle)) {
            styleNames.prepend(lnfStyle);
        }
        m_hints[QPlatformTheme::StyleNames] = styleNames;

        app->setStyle(theme);
        loadPalettes();
        break;
    }
    default:
        qWarning() << "Unknown type of change in KGlobalSettings::slotNotifyChange: " << type;
    }
}

void KHintsSettings::iconChanged(int group)
{
    KIconLoader::Group iconGroup = (KIconLoader::Group) group;
    if (iconGroup != KIconLoader::MainToolbar) {
        m_hints[QPlatformTheme::SystemIconThemeName] = readConfigValue(QStringLiteral("Icons"), QStringLiteral("Theme"), "breeze");

        return;
    }

    const int currentSize = KIconLoader::global()->currentSize(KIconLoader::MainToolbar);
    if (m_hints[QPlatformTheme::ToolBarIconSize] == currentSize) {
        return;
    }

    m_hints[QPlatformTheme::ToolBarIconSize] = currentSize;

    //If we are not a QApplication, means that we are a QGuiApplication, then we do nothing.
    if (!qobject_cast<QApplication *>(QCoreApplication::instance())) {
        return;
    }

    QWidgetList widgets = QApplication::allWidgets();
    Q_FOREACH (QWidget *widget, widgets) {
        if (qobject_cast<QToolBar *>(widget) || qobject_cast<QMainWindow *>(widget)) {
            QEvent event(QEvent::StyleChange);
            QApplication::sendEvent(widget, &event);
        }
    }
}

void KHintsSettings::updateQtSettings(KConfigGroup &cg)
{
    int flash = qBound(200, cg.readEntry("CursorBlinkRate", 1000), 2000);
    m_hints[QPlatformTheme::CursorFlashTime] = flash;

    int doubleClickInterval = cg.readEntry("DoubleClickInterval", 400);
    m_hints[QPlatformTheme::MouseDoubleClickInterval] = doubleClickInterval;

    int startDragDistance = cg.readEntry("StartDragDist", 10);
    m_hints[QPlatformTheme::StartDragDistance] = startDragDistance;

    int startDragTime = cg.readEntry("StartDragTime", 10);
    m_hints[QPlatformTheme::StartDragTime] = startDragTime;

    m_hints[QPlatformTheme::ItemViewActivateItemOnSingleClick] = cg.readEntry("SingleClick", true);

    bool showIcons = cg.readEntry("ShowIconsInMenuItems", !QApplication::testAttribute(Qt::AA_DontShowIconsInMenus));
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !showIcons);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    int wheelScrollLines = cg.readEntry("WheelScrollLines", 3);
    m_hints[QPlatformTheme::WheelScrollLines] = wheelScrollLines;
#endif
    QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (app) {
        QApplication::setWheelScrollLines(cg.readEntry("WheelScrollLines", 3));
    }
}

Qt::ToolButtonStyle KHintsSettings::toolButtonStyle(const KConfigGroup &cg) const
{
    const QString buttonStyle = cg.readEntry("ToolButtonStyle", "TextBesideIcon").toLower();
    return buttonStyle == QLatin1String("textbesideicon") ? Qt::ToolButtonTextBesideIcon
           : buttonStyle == QLatin1String("icontextright") ? Qt::ToolButtonTextBesideIcon
           : buttonStyle == QLatin1String("textundericon") ? Qt::ToolButtonTextUnderIcon
           : buttonStyle == QLatin1String("icontextbottom") ? Qt::ToolButtonTextUnderIcon
           : buttonStyle == QLatin1String("textonly") ? Qt::ToolButtonTextOnly
           : Qt::ToolButtonIconOnly;
}

void KHintsSettings::loadPalettes()
{
    qDeleteAll(m_palettes);
    m_palettes.clear();

    if (mKdeGlobals->hasGroup("Colors:View")) {
        m_palettes[QPlatformTheme::SystemPalette] = new QPalette(KColorScheme::createApplicationPalette(mKdeGlobals));
    } else {

        KConfigGroup cg(mKdeGlobals, "KDE");
        const QString looknfeel = cg.readEntry("LookAndFeelPackage", defaultLookAndFeelPackage);
        QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/look-and-feel/" + looknfeel + "/contents/colors");
        if (!path.isEmpty()) {
            m_palettes[QPlatformTheme::SystemPalette] = new QPalette(KColorScheme::createApplicationPalette(KSharedConfig::openConfig(path)));
            return;
        }

        path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("plasma/look-and-feel/org.kde.loonandfeel/contents/colors"));
        if (!path.isEmpty()) {
            m_palettes[QPlatformTheme::SystemPalette] = new QPalette(KColorScheme::createApplicationPalette(KSharedConfig::openConfig(path)));
            return;
        }

        const QString scheme = readConfigValue(QStringLiteral("General"), QStringLiteral("ColorScheme"), "Breeze").toString();
        path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "color-schemes/" + scheme + ".colors");

        if (!path.isEmpty()) {
            m_palettes[QPlatformTheme::SystemPalette] = new QPalette(KColorScheme::createApplicationPalette(KSharedConfig::openConfig(path)));
        }
    }
}

void KHintsSettings::updateCursorTheme()
{
    KConfig config(QStringLiteral("kcminputrc"));
    KConfigGroup g(&config, "Mouse");

    QString theme = g.readEntry("cursorTheme", QString());
    int size      = g.readEntry("cursorSize", -1);

    // Default cursor size is 16 points
    if (size == -1) {
        if (QScreen *s = QGuiApplication::primaryScreen()) {
            size = s->logicalDotsPerInchY() * 16 / 72;
        } else {
            size = 0;
        }
    }

#if HAVE_X11
    if (QX11Info::isPlatformX11()) {
        // Note that in X11R7.1 and earlier, calling XcursorSetTheme()
        // with a NULL theme would cause Xcursor to use "default", but
        // in 7.2 and later it will cause it to revert to the theme that
        // was configured when the application was started.
        XcursorSetTheme(QX11Info::display(), theme.isNull() ?
                        "default" : QFile::encodeName(theme).constData());
        XcursorSetDefaultSize(QX11Info::display(), size);
    }
#endif
}
