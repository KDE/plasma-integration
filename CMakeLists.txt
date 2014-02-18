cmake_minimum_required(VERSION 2.8.12)

project(FrameworkIntegration)

find_package(ECM 0.0.10 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets DBus)

set(KF5_VERSION "4.96.0")

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX FRAMEWORKINTEGRATION
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/frameworkintegration_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5FrameworkIntegrationConfigVersion.cmake")

find_package(KF5Config ${KF5_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_VERSION} REQUIRED)

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(tests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5FrameworkIntegration")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5FrameworkIntegrationConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5FrameworkIntegrationConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5FrameworkIntegrationConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5FrameworkIntegrationConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5FrameworkIntegrationTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5FrameworkIntegrationTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/frameworkintegration_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel )

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
