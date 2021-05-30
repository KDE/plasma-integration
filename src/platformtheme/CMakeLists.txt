find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "Required for updating the Cursor theme on X11"
                        URL "https://www.x.org"
                        TYPE REQUIRED
                        )
set(HAVE_X11 ${X11_FOUND})
if(HAVE_X11)
  find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED X11Extras)

  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)
endif()

configure_file(config-platformtheme.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-platformtheme.h )

# qdbusmenubar uses them
remove_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)


set(platformtheme_SRCS
    qdbusmenubar.cpp # fork of Qt's qdbusmenubar with some added setters for our convenience
    kdeplatformtheme.cpp
    kfontsettingsdata.cpp
    khintssettings.cpp
    kdeplatformfiledialoghelper.cpp
    kdeplatformfiledialogbase.cpp
    kdeplatformsystemtrayicon.cpp
    kfiletreeview.cpp
    kdirselectdialog.cpp
    kwaylandintegration.cpp
    x11integration.cpp
    main.cpp
    qxdgdesktopportalfiledialog.cpp
)

qt_add_dbus_interface(platformtheme_SRCS org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)

add_library(KDEPlasmaPlatformTheme MODULE ${platformtheme_SRCS})

target_link_libraries(KDEPlasmaPlatformTheme
    PRIVATE
        Qt::GuiPrivate
        Qt::DBus
        Qt::X11Extras
        Qt::QuickControls2
        KF5::ConfigWidgets
        KF5::ConfigCore
        KF5::IconThemes
        KF5::KIOFileWidgets # KFileFilterCombo, KDirSortFilterProxyModel, KRecentDirs
        KF5::KIOWidgets
        KF5::XmlGui
        KF5::I18n
        KF5::Notifications
        KF5::WindowSystem
        KF5::WaylandClient
        XCB::XCB
        ${QT5PLATFORMSUPPORT_LIBS}
)

if(HAVE_X11)
  target_link_libraries(KDEPlasmaPlatformTheme PRIVATE Qt::X11Extras ${X11_Xcursor_LIB} ${XCB_XCB_LIBRARY})
endif()

set_target_properties(KDEPlasmaPlatformTheme PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/platformthemes/")
install(TARGETS KDEPlasmaPlatformTheme DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platformthemes)

install(FILES fonts_global_toolbar.upd fonts_global.upd fonts_akregator.upd fonts_kate.upd DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
install(PROGRAMS fonts_global.pl fonts_akregator.pl fonts_kate.pl DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
