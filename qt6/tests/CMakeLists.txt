add_executable(qfiledialogtest6 qfiledialogtest.cpp)
target_link_libraries(qfiledialogtest6 Qt6::Core Qt6::Widgets)

add_executable(windowdecotest6 windowdecotest.cpp)
target_link_libraries(windowdecotest6 Qt6::Core Qt6::Widgets KF${QT_MAJOR_VERSION}::ConfigWidgets)

add_executable(systraymenuexclusiveactionstest6 systraymenuexclusiveactionstest.cpp)
target_link_libraries(systraymenuexclusiveactionstest6 Qt6::Core Qt6::Widgets)
