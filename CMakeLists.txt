cmake_minimum_required(VERSION 2.8.12)

project(FrameworkIntegration)

find_package(ECM 0.0.10 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

find_package(Qt5 5.2.0 CONFIG REQUIRED Widgets DBus)

include(FeatureSummary)

set(KF5_VERSION "4.96.0")

find_package(KF5Config ${KF5_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} REQUIRED)
find_package(KF5GuiAddons ${KF5_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_VERSION} REQUIRED)
find_package(KF5ItemViews ${KF5_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_VERSION} REQUIRED)

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(tests)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
