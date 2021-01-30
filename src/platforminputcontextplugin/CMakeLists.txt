# keyholdmanager has a ton of definitions where
# needing explicit qstringliteral would negatively affect
# code readability
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_library(input_plugin MODULE
    main.cpp
    plasmaimcontext.cpp
)

target_link_libraries(input_plugin
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Widgets
    Qt::GuiPrivate
    KF5::ConfigCore
)
set_target_properties(input_plugin PROPERTIES OUTPUT_NAME plasmaimplatforminputcontextplugin)

install(TARGETS input_plugin LIBRARY DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/platforminputcontexts)

# we want to share this data with some other parts of Plasma; this isn't an API for anyone to use but us
install(DIRECTORY data/ DESTINATION ${KDE_INSTALL_INCLUDEDIR}/PlasmaKeyData)
configure_file(plasma-key-data.pc.in plasma-key-data.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-key-data.pc DESTINATION ${KDE_INSTALL_LIBDIR}/pkgconfig)
