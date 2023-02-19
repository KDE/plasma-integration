find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "Required for updating the Cursor theme on X11"
                        URL "https://www.x.org"
                        TYPE REQUIRED
                        )
set(HAVE_X11 ${X11_FOUND})
if(HAVE_X11)
  if (QT_MAJOR_VERSION EQUAL "5")
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED X11Extras)
  endif()

  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)
endif()

configure_file(config-platformtheme.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-platformtheme.h )

# qdbusmenubar uses them
remove_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

set(platformtheme_SRCS
    kdeplatformtheme.cpp kdeplatformtheme.h
    kfontsettingsdata.cpp kfontsettingsdata.h
    khintssettings.cpp khintssettings.h
    kdeplatformfiledialoghelper.cpp kdeplatformfiledialoghelper.h
    kdeplatformfiledialogbase.cpp
    kdeplatformsystemtrayicon.cpp kdeplatformsystemtrayicon.h
    kfiletreeview.cpp
    kdirselectdialog.cpp
    kwaylandintegration.cpp kwaylandintegration.h
    x11integration.cpp x11integration.h
    main.cpp
    qxdgdesktopportalfiledialog.cpp
    qtquickrenderersettings.cpp
)
if (QT_MAJOR_VERSION EQUAL "5")
    list(APPEND platformtheme_SRCS qdbusmenubar.cpp) # fork of Qt's qdbusmenubar with some added setters for our convenience
endif()

qt_add_dbus_interface(platformtheme_SRCS org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)
kconfig_add_kcfg_files(platformtheme_SRCS renderersettings.kcfgc)
add_library(KDEPlasmaPlatformTheme MODULE)
if (QT_MAJOR_VERSION EQUAL "5")
    ecm_add_qtwayland_client_protocol(platformtheme_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/appmenu.xml
        BASENAME appmenu
    )

    ecm_add_qtwayland_client_protocol(platformtheme_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/server-decoration-palette.xml
        BASENAME server-decoration-palette
    )
else()
    # TODO KF6 Port D-Bus menu support
    target_compile_definitions(KDEPlasmaPlatformTheme PRIVATE -DKF6_TODO_DBUS_MENUBAR)
    qt6_generate_wayland_protocol_client_sources(KDEPlasmaPlatformTheme FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/server-decoration-palette.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/appmenu.xml
    )
endif()
target_sources(KDEPlasmaPlatformTheme PRIVATE ${platformtheme_SRCS})

target_link_libraries(KDEPlasmaPlatformTheme
    PRIVATE
        Qt::GuiPrivate
        Qt::DBus
        Qt::QuickControls2
        KF${QT_MAJOR_VERSION}::ConfigWidgets
        KF${QT_MAJOR_VERSION}::ConfigCore
        KF${QT_MAJOR_VERSION}::IconThemes
        KF${QT_MAJOR_VERSION}::KIOFileWidgets # KFileFilterCombo, KDirSortFilterProxyModel, KRecentDirs
        KF${QT_MAJOR_VERSION}::KIOWidgets
        KF${QT_MAJOR_VERSION}::KIOGui
        KF${QT_MAJOR_VERSION}::XmlGui
        KF${QT_MAJOR_VERSION}::I18n
        KF${QT_MAJOR_VERSION}::Notifications
        KF${QT_MAJOR_VERSION}::WindowSystem
        XCB::XCB
        ${QT5PLATFORMSUPPORT_LIBS}
        Qt::WaylandClient
        Qt::GuiPrivate
        Wayland::Client
        KF${QT_MAJOR_VERSION}::WaylandClient
)
if (QT_MAJOR_VERSION EQUAL "5")
    target_link_libraries(KDEPlasmaPlatformTheme PRIVATE Qt5::X11Extras)
endif()

if(HAVE_X11)
  target_link_libraries(KDEPlasmaPlatformTheme PRIVATE ${X11_Xcursor_LIB} ${XCB_XCB_LIBRARY})
endif()

set_target_properties(KDEPlasmaPlatformTheme PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/platformthemes/")
install(TARGETS KDEPlasmaPlatformTheme DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformthemes)

install(FILES fonts_global_toolbar.upd fonts_global.upd fonts_akregator.upd fonts_kate.upd DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
install(PROGRAMS fonts_global.pl fonts_akregator.pl fonts_kate.pl DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
