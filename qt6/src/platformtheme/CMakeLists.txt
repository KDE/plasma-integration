configure_file(config-platformtheme.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-platformtheme.h )

set(platformtheme_SRCS
    kdeplatformtheme.cpp kdeplatformtheme.h
    kfontsettingsdata.cpp kfontsettingsdata.h
    khintssettings.cpp khintssettings.h
    kdeplatformfiledialoghelper.cpp kdeplatformfiledialoghelper.h
    kdeplatformfiledialogbase.cpp
    kdeplatformmessagedialoghelper.cpp kdeplatformmessagedialoghelper.h
    kdeplatformcolordialoghelper.cpp kdeplatformcolordialoghelper.h
    kdeplatformsystemtrayicon.cpp kdeplatformsystemtrayicon.h
    kwaylandintegration.cpp kwaylandintegration.h
    main.cpp
    qxdgdesktopportalfiledialog.cpp
    qtquickrenderersettings.cpp
    qdbusmenubarwrapper.h
    kioopenwith.cpp
    kiodelegate.cpp
    kioopenwithxdp.cpp
)

if(WITH_X11)
    list(APPEND platformtheme_SRCS
        x11integration.cpp x11integration.h
    )
endif()

qt6_add_dbus_interface(platformtheme_SRCS org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)
kconfig_add_kcfg_files(platformtheme_SRCS renderersettings.kcfgc)
add_library(KDEPlasmaPlatformTheme6 MODULE)

# TODO KF6 Port D-Bus menu support
target_compile_definitions(KDEPlasmaPlatformTheme6 PRIVATE -DKF6_TODO_DBUS_MENUBAR)

qt6_generate_wayland_protocol_client_sources(KDEPlasmaPlatformTheme6
    PRIVATE_CODE
    FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/server-decoration-palette.xml
)

target_sources(KDEPlasmaPlatformTheme6 PRIVATE ${platformtheme_SRCS})

target_link_libraries(KDEPlasmaPlatformTheme6
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::DBus
        Qt6::QuickControls2
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::IconThemes
        KF6::KIOFileWidgets # KFileFilterCombo, KDirSortFilterProxyModel, KRecentDirs
        KF6::KIOWidgets
        KF6::KIOGui
        KF6::StatusNotifierItem
        KF6::I18n
        KF6::Notifications
        KF6::WindowSystem
        KF6::GuiAddons
        Qt6::WaylandClient
        Wayland::Client
)

target_link_libraries(KDEPlasmaPlatformTheme6 PRIVATE KF6::ColorScheme)

if(WITH_X11)
    target_link_libraries(KDEPlasmaPlatformTheme6 PRIVATE X11::Xcursor XCB::XCB)
endif()

set_target_properties(KDEPlasmaPlatformTheme6 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/platformthemes/")
install(TARGETS KDEPlasmaPlatformTheme6 DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformthemes)
