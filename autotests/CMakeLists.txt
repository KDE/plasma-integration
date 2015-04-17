include(ECMMarkAsTest)

find_package(Qt5Test ${REQUIRED_QT_VERSION} CONFIG QUIET)
find_package(Qt5Qml ${REQUIRED_QT_VERSION} CONFIG QUIET)

if(NOT Qt5Test_FOUND)
    message(STATUS "Qt5Test not found, autotests will not be built.")
    return()
endif()

if(NOT Qt5Qml_FOUND)
    message(STATUS "Qt5Qml not found, QML autotests will not be built.")
endif()

include_directories( ${Qt5Gui_PRIVATE_INCLUDE_DIRS} )

set(CONFIGFILE "${CMAKE_CURRENT_SOURCE_DIR}/kdeplatformtheme_kdeglobals")
set(CHANGED_CONFIGFILE "${CMAKE_CURRENT_SOURCE_DIR}/kdeplatformtheme_changed_kdeglobals")
configure_file(kdeplatformtheme_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/kdeplatformtheme_config.h)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

macro(FRAMEWORKINTEGRATION_TESTS _testname)
    add_executable(${_testname} ${_testname}.cpp ${ARGN})
    set_target_properties(${_testname} PROPERTIES COMPILE_FLAGS "-DUNIT_TEST")
    add_test(frameworkintegration-${_testname} ${_testname})
    ecm_mark_as_test(${_testname})
    target_link_libraries(${_testname} Qt5::Test Qt5::DBus KF5::ConfigWidgets KF5::ConfigCore KF5::IconThemes KF5::Style KF5::KIOFileWidgets KF5::I18n KF5::Notifications)
endmacro()

frameworkintegration_tests(
  kdeplatformtheme_unittest
  ../src/platformtheme/kdeplatformtheme.cpp
  ../src/platformtheme/kfontsettingsdata.cpp
  ../src/platformtheme/khintssettings.cpp
  ../src/platformtheme/kdeplatformfiledialoghelper.cpp
  ../src/platformtheme/kdeplatformfiledialogbase.cpp
  ../src/platformtheme/kdeplatformsystemtrayicon.cpp
  ../src/platformtheme/kdirselectdialog.cpp
  ../src/platformtheme/kfiletreeview.cpp
)

frameworkintegration_tests(
  kfontsettingsdata_unittest
  ../src/platformtheme/kfontsettingsdata.cpp
)

frameworkintegration_tests(
  kstyle_unittest
)

frameworkintegration_tests(
  kfiledialog_unittest
)

frameworkintegration_tests(
  ksni_unittest
)

if(Qt5Qml_FOUND)
    frameworkintegration_tests(kfiledialogqml_unittest)
    target_link_libraries(kfiledialogqml_unittest Qt5::Qml)
endif()
