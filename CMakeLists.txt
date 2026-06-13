cmake_minimum_required(VERSION 3.19)
project(ui LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(ui
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    flowdata.h
    flowwidget.h
    flowwidget.cpp
    flowtemplates.h
    flowtemplates.cpp
    flowdemopage.h
    flowdemopage.cpp
    circledisplaywidget.h
    circledisplaywidget.cpp
    circledemopage.h
    circledemopage.cpp
    circleselectdemopage.h
    circleselectdemopage.cpp
    circlerotatedemopage.h
    circlerotatedemopage.cpp
)

target_link_libraries(ui
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS ui
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ui
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
