set(QT_MAJOR_VERSION 6)

include(KDEInstallDirs6)
include(KDECMakeSettings)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED Widgets DBus QuickControls2 WaylandClient)
find_package(Qt${QT_MAJOR_VERSION}Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)

# https://bugreports.qt.io/browse/QTBUG-114706
add_library(Qt::Core ALIAS Qt6::Core)
add_library(Qt::Network ALIAS Qt6::Network)
add_library(Qt::OpenGL ALIAS Qt6::OpenGL)
add_library(Qt::Gui ALIAS Qt6::Gui)

find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
                Config ConfigWidgets I18n IconThemes KIO Notifications
                WidgetsAddons WindowSystem ConfigWidgets Wayland GuiAddons
                XmlGui
)

find_package(KF6ColorScheme ${KF_MIN_VERSION} REQUIRED CONFIG)

# TODO this should be Breeze6 so as to not overlap with Breeze (which is Qt5)
find_package(Breeze 5 CONFIG)
set_package_properties(Breeze PROPERTIES
                       TYPE REQUIRED
                       PURPOSE "For setting the default QStyle name")

ecm_set_disabled_deprecation_versions(QT 5.15.2
    KF 5.101
)

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
