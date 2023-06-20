set(QT_MAJOR_VERSION 5)

include(KDEInstallDirs5)
include(KDECMakeSettings)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Widgets DBus QuickControls2 WaylandClient)
find_package(Qt${QT_MAJOR_VERSION}Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED X11Extras)

# https://bugreports.qt.io/browse/QTBUG-114706
add_library(Qt::Core ALIAS Qt5::Core)
add_library(Qt::Network ALIAS Qt5::Network)
add_library(Qt::Gui ALIAS Qt5::Gui)

find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
                Config ConfigWidgets I18n IconThemes KIO Notifications
                WidgetsAddons WindowSystem ConfigWidgets Wayland GuiAddons
                XmlGui
)

find_package(KF5ConfigWidgets ${KF_MIN_VERSION} REQUIRED CONFIG)

find_package(QtWaylandScanner ${QT_MIN_VERSION} REQUIRED)

find_package(Breeze 5 CONFIG)
set_package_properties(Breeze PROPERTIES
                       TYPE REQUIRED
                       PURPOSE "For setting the default QStyle name")

# dependencies for QPA plugin
find_package(Qt5ThemeSupport REQUIRED)
set(QT5PLATFORMSUPPORT_LIBS Qt5ThemeSupport::Qt5ThemeSupport)
ecm_set_disabled_deprecation_versions(QT 5.15.2
    KF 5.101
)

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
