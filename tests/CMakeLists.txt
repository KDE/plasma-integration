add_executable(qfiledialogtest qfiledialogtest.cpp)
target_link_libraries(qfiledialogtest Qt::Core Qt::Widgets)

add_executable(windowdecotest windowdecotest.cpp)
target_link_libraries(windowdecotest Qt::Core Qt::Widgets KF5::ConfigWidgets)

add_executable(systraymenuexclusiveactionstest systraymenuexclusiveactionstest.cpp)
target_link_libraries(systraymenuexclusiveactionstest Qt::Core Qt::Widgets)
