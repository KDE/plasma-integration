cmake_minimum_required(VERSION 2.8.12)

project(FrameworkIntegration)
set(PROJECT_VERSION "5.6.90")
set(PROJECT_VERSION_MAJOR 5)

include(FeatureSummary)
find_package(ECM 5.17.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})


include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.5.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets DBus)

set(KF5_VERSION "5.18.0") # handled by release scripts
set(KF5_DEP_VERSION "5.17.0") # handled by release scripts

find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Wayland 5.5 CONFIG REQUIRED)

add_definitions(-DTRANSLATION_DOMAIN=\"plasmaintegration5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(tests)


feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
message("** plasma-integration uses Noto Sans (https://www.google.com/get/noto/) and Oxygen Mono (http://download.kde.org/stable/plasma/5.4.0/oxygen-fonts-5.4.0.tar.xz) fonts, ensure these are installed for use at runtime")
