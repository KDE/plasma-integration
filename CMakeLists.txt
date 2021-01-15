cmake_minimum_required(VERSION 3.0)

project(PlasmaIntegration)
set(PROJECT_VERSION "5.20.80")
set(PROJECT_VERSION_MAJOR 5)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.74")

find_package(ECM ${KF5_MIN_VERSION}  REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(GenerateExportHeader)
include(KDEClangFormat)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Widgets DBus X11Extras QuickControls2)
find_package(Qt5Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
                Config ConfigWidgets I18n IconThemes KIO Notifications Wayland
                WidgetsAddons WindowSystem ConfigWidgets
)

find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
    DESCRIPTION "X protocol C-language Binding"
    URL "https://xcb.freedesktop.org"
    TYPE REQUIRED
    PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

find_package(Breeze 5 CONFIG)
set_package_properties(Breeze PROPERTIES
                       TYPE REQUIRED
                       PURPOSE "For setting the default QStyle name")

# dependencies for QPA plugin
find_package(Qt5ThemeSupport REQUIRED)
set(QT5PLATFORMSUPPORT_LIBS Qt5ThemeSupport::Qt5ThemeSupport)

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
    URL "https://sourcefoundry.org/hack/"
    TYPE RUNTIME
)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
