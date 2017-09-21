

set(kirigamiplasmaintegration_SRCS
    kirigamiplasmatheme.cpp
)


add_library(kirigamiplasmaintegration MODULE ${kirigamiplasmaintegration_SRCS})

target_link_libraries(kirigamiplasmaintegration
    PUBLIC
        Qt5::Core
        KF5::Kirigami2
    PRIVATE
        Qt5::Qml
        Qt5::Quick
        KF5::ConfigWidgets
)

install(TARGETS kirigamiplasmaintegration DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kirigami)

