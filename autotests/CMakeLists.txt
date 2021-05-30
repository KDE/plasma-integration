include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)

find_package(Qt5Test ${QT_MIN_VERSION} CONFIG QUIET)
find_package(Qt5Qml ${QT_MIN_VERSION} CONFIG QUIET)

if(NOT Qt5Test_FOUND)
    message(STATUS "Qt5Test not found, autotests will not be built.")
    return()
endif()

if(NOT Qt5Qml_FOUND)
    message(STATUS "Qt5Qml not found, QML autotests will not be built.")
endif()

set(CONFIGFILE "${CMAKE_CURRENT_SOURCE_DIR}/kdeplatformtheme_kdeglobals")
set(CHANGED_CONFIGFILE "${CMAKE_CURRENT_SOURCE_DIR}/kdeplatformtheme_changed_kdeglobals")
configure_file(kdeplatformtheme_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/kdeplatformtheme_config.h)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
# qdbusmenubar uses them
remove_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

macro(FRAMEWORKINTEGRATION_TESTS _testname)
    add_executable(${_testname} ${_testname}.cpp ${ARGN})
    set_target_properties(${_testname} PROPERTIES COMPILE_FLAGS "-DUNIT_TEST")
    add_test(NAME frameworkintegration-${_testname} COMMAND ${_testname})
    ecm_mark_as_test(${_testname})
    ecm_mark_nongui_executable(${_testname})
    target_include_directories(${_testname} PRIVATE ${CMAKE_BINARY_DIR}/src/platformtheme)
    target_link_libraries(${_testname} Qt::GuiPrivate Qt::Test Qt::DBus Qt::X11Extras Qt::QuickControls2 ${QT5PLATFORMSUPPORT_LIBS} KF5::ConfigWidgets KF5::ConfigCore KF5::IconThemes KF5::KIOFileWidgets KF5::I18n KF5::Notifications KF5::WindowSystem KF5::WaylandClient XCB::XCB)
endmacro()

set(dbus_interface)
qt_add_dbus_interface(dbus_interface ../src/platformtheme/org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)

set(platformThemeSRCS
  ../src/platformtheme/qdbusmenubar.cpp # fork of Qt's qdbusmenubar with some added setters for our convenience
  ../src/platformtheme/kdeplatformtheme.cpp
  ../src/platformtheme/kfontsettingsdata.cpp
  ../src/platformtheme/khintssettings.cpp
  ../src/platformtheme/kdeplatformfiledialoghelper.cpp
  ../src/platformtheme/kdeplatformfiledialogbase.cpp
  ../src/platformtheme/kdeplatformsystemtrayicon.cpp
  ../src/platformtheme/kdirselectdialog.cpp
  ../src/platformtheme/kfiletreeview.cpp
  ../src/platformtheme/kwaylandintegration.cpp
  ../src/platformtheme/x11integration.cpp
  ../src/platformtheme/qxdgdesktopportalfiledialog.cpp
  ${dbus_interface}
)

frameworkintegration_tests(
  kdeplatformtheme_unittest
  ${platformThemeSRCS}
)

frameworkintegration_tests(
  kfontsettingsdata_unittest
  ../src/platformtheme/kfontsettingsdata.cpp
)

frameworkintegration_tests(
  kfiledialog_unittest
)

frameworkintegration_tests(
  ksni_unittest
)

frameworkintegration_tests(
  kdeplatformsystemtrayicon_unittest
  ../src/platformtheme/kdeplatformsystemtrayicon.cpp
  ${dbus_interface}
)

frameworkintegration_tests(
  kdirselectdialog_unittest
  ../src/platformtheme/kdeplatformfiledialogbase.cpp
  ../src/platformtheme/kdirselectdialog.cpp
  ../src/platformtheme/kfiletreeview.cpp
)

frameworkintegration_tests(
  khintssettings_unittest
  ../src/platformtheme/khintssettings.cpp
)

if(Qt5Qml_FOUND)
    add_test(NAME qmltests COMMAND qmltestrunner WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    frameworkintegration_tests(kfiledialogqml_unittest)
    target_link_libraries(kfiledialogqml_unittest Qt::Qml)
endif()
