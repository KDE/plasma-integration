if(NOT APPLE)
  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES
    TYPE RECOMMENDED
    PURPOSE "Required for flushing the XCB connection on the X11 Platform"
  )
  set(HAVE_X11 ${XCB_XCB_FOUND})
  if (XCB_XCB_FOUND)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED X11Extras)
  endif()
else()
    set(HAVE_X11 FALSE)
endif()

configure_file(config-platformtheme.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-platformtheme.h )

set(platformtheme_SRCS
    kdeplatformtheme.cpp
    kfontsettingsdata.cpp
    khintssettings.cpp
    kdeplatformfiledialoghelper.cpp
    kdeplatformfiledialogbase.cpp
    kdeplatformsystemtrayicon.cpp
    kfiletreeview.cpp
    kdirselectdialog.cpp
    main.cpp
)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_library(KDEPlatformTheme MODULE ${platformtheme_SRCS})

# We link to KIO to pull in the qfiledialog hooks
target_link_libraries(KDEPlatformTheme
    PRIVATE
        Qt5::DBus
        KF5::ConfigWidgets
        KF5::ConfigCore
        KF5::IconThemes
        KF5::KIOFileWidgets
        KF5::I18n
        KF5::Notifications
)

if(HAVE_X11)
  target_link_libraries(KDEPlatformTheme PRIVATE Qt5::X11Extras XCB::XCB)
endif()

install(TARGETS KDEPlatformTheme DESTINATION ${QT_PLUGIN_INSTALL_DIR}/platformthemes)
