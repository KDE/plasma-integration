include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)

find_package(Qt${QT_MAJOR_VERSION}Test ${QT_MIN_VERSION} CONFIG QUIET)
find_package(Qt${QT_MAJOR_VERSION}Qml ${QT_MIN_VERSION} CONFIG QUIET)

if(NOT Qt${QT_MAJOR_VERSION}Test_FOUND)
    message(STATUS "Qt${QT_MAJOR_VERSION}Test not found, autotests will not be built.")
    return()
endif()

if(NOT Qt${QT_MAJOR_VERSION}Qml_FOUND)
    message(STATUS "Qt${QT_MAJOR_VERSION}Qml not found, QML autotests will not be built.")
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
    target_link_libraries(${_testname} Qt::GuiPrivate Qt::Test Qt::DBus Qt::QuickControls2 ${QT5PLATFORMSUPPORT_LIBS} KF5::ConfigWidgets KF5::ConfigCore KF5::IconThemes KF5::KIOFileWidgets KF5::I18n KF5::Notifications KF5::WindowSystem Qt::WaylandClient XCB::XCB Wayland::Client KF5::WaylandClient)
    if (QT_MAJOR_VERSION EQUAL "5")
        target_link_libraries(${_testname} Qt5::X11Extras)
    endif()
endmacro()

set(dbus_interface)
qt_add_dbus_interface(dbus_interface ../src/platformtheme/org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)

frameworkintegration_tests(
    kdeplatformtheme_unittest
)
set(wayland_interfaces)
if (QT_MAJOR_VERSION EQUAL "5")
    ecm_add_qtwayland_client_protocol(wayland_interfaces
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/appmenu.xml
        BASENAME appmenu
    )

    ecm_add_qtwayland_client_protocol(wayland_interfaces
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/server-decoration-palette.xml
        BASENAME server-decoration-palette
    )
else()
    qt6_generate_wayland_protocol_client_sources(kdeplatformtheme_unittest FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/appmenu.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/server-decoration-palette.xml
    )
endif()

target_sources(kdeplatformtheme_unittest PRIVATE
  ../src/platformtheme/kdeplatformtheme.cpp ../src/platformtheme/kdeplatformtheme.h
  ../src/platformtheme/kfontsettingsdata.cpp ../src/platformtheme/kfontsettingsdata.h
  ../src/platformtheme/khintssettings.cpp ../src/platformtheme/khintssettings.h
  ../src/platformtheme/kdeplatformfiledialoghelper.cpp ../src/platformtheme/kdeplatformfiledialoghelper.h
  ../src/platformtheme/kdeplatformfiledialogbase.cpp
  ../src/platformtheme/kdeplatformsystemtrayicon.cpp ../src/platformtheme/kdeplatformsystemtrayicon.h
  ../src/platformtheme/kdirselectdialog.cpp
  ../src/platformtheme/kfiletreeview.cpp
  ../src/platformtheme/kwaylandintegration.cpp ../src/platformtheme/kwaylandintegration.h
  ../src/platformtheme/x11integration.cpp ../src/platformtheme/x11integration.h
  ../src/platformtheme/qxdgdesktopportalfiledialog.cpp
  ${dbus_interface}
  ${wayland_interfaces}
)
if (QT_MAJOR_VERSION EQUAL "5")
  target_sources(kdeplatformtheme_unittest PRIVATE
    ../src/platformtheme/qdbusmenubar.cpp # fork of Qt's qdbusmenubar with some added setters for our convenience
  )
else()
  # TODO KF6 Port D-Bus menu support
  target_compile_definitions(kdeplatformtheme_unittest PRIVATE -DKF6_TODO_DBUS_MENUBAR)
endif()

frameworkintegration_tests(
  kfontsettingsdata_unittest
  ../src/platformtheme/kfontsettingsdata.cpp ../src/platformtheme/kfontsettingsdata.h
)

frameworkintegration_tests(
  kfiledialog_unittest
)

frameworkintegration_tests(
  ksni_unittest
)

frameworkintegration_tests(
  kdeplatformsystemtrayicon_unittest
  ../src/platformtheme/kdeplatformsystemtrayicon.cpp ../src/platformtheme/kdeplatformsystemtrayicon.h
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
  ../src/platformtheme/khintssettings.cpp ../src/platformtheme/khintssettings.h
)

if(Qt${QT_MAJOR_VERSION}Qml_FOUND)
    add_test(NAME qmltests COMMAND qmltestrunner WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    frameworkintegration_tests(kfiledialogqml_unittest)
    target_link_libraries(kfiledialogqml_unittest Qt::Qml)
endif()
