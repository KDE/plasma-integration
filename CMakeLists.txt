cmake_minimum_required(VERSION 2.8.12)

project(PlasmaIntegration)
set(PROJECT_VERSION "5.8.9")
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
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets DBus X11Extras)

set(KF5_VERSION "5.18.0") # handled by release scripts
set(KF5_DEP_VERSION "5.17.0") # handled by release scripts

find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Wayland 5.5 CONFIG REQUIRED)

find_package(Breeze ${PROJECT_VERSION} CONFIG)
set_package_properties(Breeze PROPERTIES
                       TYPE REQUIRED
                       PURPOSE "For setting the default QStyle name")

add_definitions(-DTRANSLATION_DOMAIN=\"plasmaintegration5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(tests)

find_package(FontNotoSans)
set_package_properties(FontNotoSans PROPERTIES
    PURPOSE "Default sans-serif font -- this is not detected automatically, pass -DCMAKE_DISABLE_FIND_PACKAGE_FontNotoSans=true to mark it ignored."
    URL "https://www.google.com/get/noto/"
    TYPE RUNTIME
)
find_package(FontHack)
set_package_properties(FontHack PROPERTIES
    PURPOSE "Default monospace font -- this is not detected automatically, pass -DCMAKE_DISABLE_FIND_PACKAGE_FontHack=true to mark it ignored."
    URL "http://sourcefoundry.org/hack/"
    TYPE RUNTIME
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
