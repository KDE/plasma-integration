add_executable(qfiledialogtest qfiledialogtest.cpp)
target_link_libraries(qfiledialogtest Qt5::Core Qt5::Widgets)

add_executable(windowdecotest windowdecotest.cpp)
target_link_libraries(windowdecotest Qt5::Core Qt5::Widgets KF5::ConfigWidgets)
