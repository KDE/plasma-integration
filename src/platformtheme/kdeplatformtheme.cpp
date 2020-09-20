/*  This file is part of the KDE libraries
 *  Copyright 2013 Kevin Ottens <ervin+bluesystems@kde.org>
 *  Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  Copyright 2014 Lukáš Tinkl <ltinkl@redhat.com>
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

#include <config-platformtheme.h>

#include "kdeplatformtheme.h"
#include "kfontsettingsdata.h"
#include "khintssettings.h"
#include "kdeplatformfiledialoghelper.h"
#include "kdeplatformsystemtrayicon.h"
#include "kwaylandintegration.h"
#include "x11integration.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QVariant>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>
#include <QtQuickControls2/QQuickStyle>

#include <kiconengine.h>
#include <kiconloader.h>
#include <kstandardshortcut.h>
#include <KStandardGuiItem>
#include <KLocalizedString>
#include <KWindowSystem>
#include <KIO/Global>

#include "qdbusmenubar_p.h"
#include "qxdgdesktopportalfiledialog_p.h"
#include "colordialog.h"

static const QByteArray s_x11AppMenuServiceNamePropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_SERVICE_NAME");
static const QByteArray s_x11AppMenuObjectPathPropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_OBJECT_PATH");

static bool checkDBusGlobalMenuAvailable()
{
    if (qEnvironmentVariableIsSet("KDE_NO_GLOBAL_MENU")) {
        return false;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();
    QString registrarService = QStringLiteral("com.canonical.AppMenu.Registrar");
    return connection.interface()->isServiceRegistered(registrarService);
}

static bool isDBusGlobalMenuAvailable()
{
    static bool dbusGlobalMenuAvailable = checkDBusGlobalMenuAvailable();
    return dbusGlobalMenuAvailable;
}

KdePlatformTheme::KdePlatformTheme()
{
    loadSettings();

    if (KWindowSystem::isPlatformWayland()) {
        m_kwaylandIntegration.reset(new KWaylandIntegration());
        m_kwaylandIntegration->init();
    }

#if HAVE_X11
    if (KWindowSystem::isPlatformX11()) {
        m_x11Integration.reset(new X11Integration());
        m_x11Integration->init();
    }
#endif

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
    setQtQuickControlsTheme();
}

KdePlatformTheme::~KdePlatformTheme()
{
    delete m_fontsData;
    delete m_hints;
}

QVariant KdePlatformTheme::themeHint(QPlatformTheme::ThemeHint hintType) const
{
    QVariant hint = m_hints->hint(hintType);
    if (hint.isValid()) {
        return hint;
    } else {
        return QPlatformTheme::themeHint(hintType);
    }
}

QIcon KdePlatformTheme::fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    if (iconOptions.testFlag(DontUseCustomDirectoryIcons) && fileInfo.isDir()) {
        return QIcon::fromTheme(QLatin1String("inode-directory"));
    }

    return QIcon::fromTheme(KIO::iconNameForUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath())));
}

const QPalette *KdePlatformTheme::palette(Palette type) const
{
    QPalette *palette = m_hints->palette(type);
    if (palette) {
        return palette;
    } else {
        return QPlatformTheme::palette(type);
    }
}

const QFont *KdePlatformTheme::font(Font type) const
{
    KFontSettingsData::FontTypes fdtype;
    switch (type) {
    case SystemFont:
        fdtype = KFontSettingsData::GeneralFont; break;
    case MenuFont:
    case MenuBarFont:
    case MenuItemFont:
        fdtype = KFontSettingsData::MenuFont; break;
    case MessageBoxFont:
    case LabelFont:
    case TipLabelFont:
    case StatusBarFont:
    case PushButtonFont:
    case ItemViewFont:
    case ListViewFont:
    case HeaderViewFont:
    case ListBoxFont:
    case ComboMenuItemFont:
    case ComboLineEditFont:
        fdtype = KFontSettingsData::GeneralFont; break;
    case TitleBarFont:
    case MdiSubWindowTitleFont:
    case DockWidgetTitleFont:
        fdtype = KFontSettingsData::WindowTitleFont; break;
    case SmallFont:
    case MiniFont:
        fdtype = KFontSettingsData::SmallestReadableFont; break;
    case FixedFont:
        fdtype = KFontSettingsData::FixedFont; break;
    case ToolButtonFont:
        fdtype = KFontSettingsData::ToolbarFont; break;
    default:
        fdtype = KFontSettingsData::GeneralFont; break;
    }

    return m_fontsData->font(fdtype);
}

QIconEngine *KdePlatformTheme::createIconEngine(const QString &iconName) const
{
    return new KIconEngine(iconName, KIconLoader::global());
}

void KdePlatformTheme::loadSettings()
{
    m_fontsData = new KFontSettingsData;
    m_hints = new KHintsSettings;
}

QList<QKeySequence> KdePlatformTheme::keyBindings(QKeySequence::StandardKey key) const
{
    switch (key) {
    case QKeySequence::HelpContents:
        return KStandardShortcut::shortcut(KStandardShortcut::Help);
    case QKeySequence::WhatsThis:
        return KStandardShortcut::shortcut(KStandardShortcut::WhatsThis);
    case QKeySequence::Open:
        return KStandardShortcut::shortcut(KStandardShortcut::Open);
    case QKeySequence::Close:
        return KStandardShortcut::shortcut(KStandardShortcut::Close);
    case QKeySequence::Save:
        return KStandardShortcut::shortcut(KStandardShortcut::Save);
    case QKeySequence::New:
        return KStandardShortcut::shortcut(KStandardShortcut::New);
    case QKeySequence::Cut:
        return KStandardShortcut::shortcut(KStandardShortcut::Cut);
    case QKeySequence::Copy:
        return KStandardShortcut::shortcut(KStandardShortcut::Copy);
    case QKeySequence::Paste:
        return KStandardShortcut::shortcut(KStandardShortcut::Paste);
    case QKeySequence::Undo:
        return KStandardShortcut::shortcut(KStandardShortcut::Undo);
    case QKeySequence::Redo:
        return KStandardShortcut::shortcut(KStandardShortcut::Redo);
    case QKeySequence::Back:
        return KStandardShortcut::shortcut(KStandardShortcut::Back);
    case QKeySequence::Forward:
        return KStandardShortcut::shortcut(KStandardShortcut::Forward);
    case QKeySequence::Refresh:
        return KStandardShortcut::shortcut(KStandardShortcut::Reload);
    case QKeySequence::ZoomIn:
        return KStandardShortcut::shortcut(KStandardShortcut::ZoomIn);
    case QKeySequence::ZoomOut:
        return KStandardShortcut::shortcut(KStandardShortcut::ZoomOut);
    case QKeySequence::Print:
        return KStandardShortcut::shortcut(KStandardShortcut::Print);
    case QKeySequence::Find:
        return KStandardShortcut::shortcut(KStandardShortcut::Find);
    case QKeySequence::FindNext:
        return KStandardShortcut::shortcut(KStandardShortcut::FindNext);
    case QKeySequence::FindPrevious:
        return KStandardShortcut::shortcut(KStandardShortcut::FindPrev);
    case QKeySequence::Replace:
        return KStandardShortcut::shortcut(KStandardShortcut::Replace);
    case QKeySequence::SelectAll:
        return KStandardShortcut::shortcut(KStandardShortcut::SelectAll);
    case QKeySequence::MoveToNextWord:
        return KStandardShortcut::shortcut(KStandardShortcut::ForwardWord);
    case QKeySequence::MoveToPreviousWord:
        return KStandardShortcut::shortcut(KStandardShortcut::BackwardWord);
    case QKeySequence::MoveToNextPage:
        return KStandardShortcut::shortcut(KStandardShortcut::Next);
    case QKeySequence::MoveToPreviousPage:
        return KStandardShortcut::shortcut(KStandardShortcut::Prior);
    case QKeySequence::MoveToStartOfLine:
        return KStandardShortcut::shortcut(KStandardShortcut::BeginningOfLine);
    case QKeySequence::MoveToEndOfLine:
        return KStandardShortcut::shortcut(KStandardShortcut::EndOfLine);
    case QKeySequence::MoveToStartOfDocument:
        return KStandardShortcut::shortcut(KStandardShortcut::Begin);
    case QKeySequence::MoveToEndOfDocument:
        return KStandardShortcut::shortcut(KStandardShortcut::End);
    case QKeySequence::SaveAs:
        return KStandardShortcut::shortcut(KStandardShortcut::SaveAs);
    case QKeySequence::Preferences:
        return KStandardShortcut::shortcut(KStandardShortcut::Preferences);
    case QKeySequence::Quit:
        return KStandardShortcut::shortcut(KStandardShortcut::Quit);
    case QKeySequence::FullScreen:
        return KStandardShortcut::shortcut(KStandardShortcut::FullScreen);
    case QKeySequence::Deselect:
        return KStandardShortcut::shortcut(KStandardShortcut::Deselect);
    case QKeySequence::DeleteStartOfWord:
        return KStandardShortcut::shortcut(KStandardShortcut::DeleteWordBack);
    case QKeySequence::DeleteEndOfWord:
        return KStandardShortcut::shortcut(KStandardShortcut::DeleteWordForward);
    case QKeySequence::NextChild:
        return KStandardShortcut::shortcut(KStandardShortcut::TabNext);
    case QKeySequence::PreviousChild:
        return KStandardShortcut::shortcut(KStandardShortcut::TabPrev);
    case QKeySequence::Delete:
        return KStandardShortcut::shortcut(KStandardShortcut::MoveToTrash);
    default:
        return QPlatformTheme::keyBindings(key);
    }
}

bool KdePlatformTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const
{
    if (type == QPlatformTheme::FileDialog && qobject_cast<QApplication*>(QCoreApplication::instance())) {
        return true;
    } else if (type == QPlatformTheme::ColorDialog) {
        return true;
    }
    return false;
}

QString KdePlatformTheme::standardButtonText(int button) const
{
    switch (static_cast<QPlatformDialogHelper::StandardButton>(button)) {
    case QPlatformDialogHelper::NoButton:
        qWarning() << Q_FUNC_INFO << "Unsupported standard button:" << button;
        return QString();
    case QPlatformDialogHelper::Ok:
        return KStandardGuiItem::ok().text();
    case QPlatformDialogHelper::Save:
        return KStandardGuiItem::save().text();
    case QPlatformDialogHelper::SaveAll:
        return i18nc("@action:button", "Save All");
    case QPlatformDialogHelper::Open:
        return KStandardGuiItem::open().text();
    case QPlatformDialogHelper::Yes:
        return KStandardGuiItem::yes().text();
    case QPlatformDialogHelper::YesToAll:
        return i18nc("@action:button", "Yes to All");
    case QPlatformDialogHelper::No:
        return KStandardGuiItem::no().text();
    case QPlatformDialogHelper::NoToAll:
        return i18nc("@action:button", "No to All");
    case QPlatformDialogHelper::Abort:
        // FIXME KStandardGuiItem::stop() doesn't seem right here
        return i18nc("@action:button", "Abort");
    case QPlatformDialogHelper::Retry:
        return i18nc("@action:button", "Retry");
    case QPlatformDialogHelper::Ignore:
        return i18nc("@action:button", "Ignore");
    case QPlatformDialogHelper::Close:
        return KStandardGuiItem::close().text();
    case QPlatformDialogHelper::Cancel:
        return KStandardGuiItem::cancel().text();
    case QPlatformDialogHelper::Discard:
        return KStandardGuiItem::discard().text();
    case QPlatformDialogHelper::Help:
        return KStandardGuiItem::help().text();
    case QPlatformDialogHelper::Apply:
        return KStandardGuiItem::apply().text();
    case QPlatformDialogHelper::Reset:
        return KStandardGuiItem::reset().text();
    case QPlatformDialogHelper::RestoreDefaults:
        return KStandardGuiItem::defaults().text();
    default:
        return QPlatformTheme::defaultStandardButtonText(button);
    }
}

QPlatformDialogHelper *KdePlatformTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const
{
    switch (type) {
    case QPlatformTheme::FileDialog:
        if (useXdgDesktopPortal()) {
            return new QXdgDesktopPortalFileDialog;
        }
        return new KDEPlatformFileDialogHelper;
    case QPlatformTheme::FontDialog:
    case QPlatformTheme::ColorDialog:
        return new ColorDialogHelper;
    case QPlatformTheme::MessageDialog:
    default:
        return nullptr;
    }
}

QPlatformSystemTrayIcon *KdePlatformTheme::createPlatformSystemTrayIcon() const
{
    return new KDEPlatformSystemTrayIcon;
}

QPlatformMenuBar *KdePlatformTheme::createPlatformMenuBar() const
{
    if (isDBusGlobalMenuAvailable()) {
        auto *menu = new QDBusMenuBar();

        QObject::connect(menu, &QDBusMenuBar::windowChanged, menu, [this, menu](QWindow *newWindow, QWindow *oldWindow) {
            const QString &serviceName = QDBusConnection::sessionBus().baseService();
            const QString &objectPath = menu->objectPath();

            if (m_x11Integration) {
                if (oldWindow) {
                    m_x11Integration->setWindowProperty(oldWindow, s_x11AppMenuServiceNamePropertyName, {});
                    m_x11Integration->setWindowProperty(oldWindow, s_x11AppMenuObjectPathPropertyName, {});
                }

                if (newWindow) {
                    m_x11Integration->setWindowProperty(newWindow, s_x11AppMenuServiceNamePropertyName, serviceName.toUtf8());
                    m_x11Integration->setWindowProperty(newWindow, s_x11AppMenuObjectPathPropertyName, objectPath.toUtf8());
                }
            }

            if (m_kwaylandIntegration) {
                if (oldWindow) {
                    m_kwaylandIntegration->setAppMenu(oldWindow, QString(), QString());
                }

                if (newWindow) {
                    m_kwaylandIntegration->setAppMenu(newWindow, serviceName, objectPath);
                }
            }
        });

        return menu;
    }

    return nullptr;
}

//force QtQuickControls2 to use the desktop theme as default
void KdePlatformTheme::setQtQuickControlsTheme()
{
    //if the user is running only a QGuiApplication, explicitly unset the QQC1 desktop style and abort
    //as this style is all about QWidgets and we know setting this will make it crash
    if (!qobject_cast<QApplication*>(qApp)) {
        if (qgetenv("QT_QUICK_CONTROLS_1_STYLE").right(7) == "Desktop") {
            qunsetenv("QT_QUICK_CONTROLS_1_STYLE");
        }
        return;
    }
    //if the user has explicitly set something else, don't meddle
    if (!QQuickStyle::name().isEmpty()) {
        return;
    }
    QQuickStyle::setStyle(QLatin1String("org.kde.desktop"));
}

bool KdePlatformTheme::useXdgDesktopPortal()
{
    static bool usePortal = qEnvironmentVariableIntValue("PLASMA_INTEGRATION_USE_PORTAL") == 1;
    return usePortal;
}
