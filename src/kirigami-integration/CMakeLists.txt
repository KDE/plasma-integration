

set(plasmadesktopintegration_SRCS
    plasmadesktoptheme.cpp
    plasmadesktopthemefactory.cpp
)


add_library(plasmadesktopintegration MODULE ${plasmadesktopintegration_SRCS})

target_link_libraries(plasmadesktopintegration
    PUBLIC
        Qt5::Core
        KF5::Kirigami2
    PRIVATE
        Qt5::Qml
        Qt5::Quick
        KF5::ConfigWidgets
        KF5::IconThemes
)

install(TARGETS plasmadesktopintegration DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kirigami)

