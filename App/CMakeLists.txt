cmake_minimum_required(VERSION 3.16)
project(OurPaintApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

include(GNUInstallDirs)

if (WIN32)
    set(APP_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
    set_source_files_properties(${APP_ICON_RC} PROPERTIES LANGUAGE RC)
endif()

add_executable(OurPaintApp WIN32
        main.cc
        CustomConsole.cc
        RenderEngine.cc
        UIManager.cc
        ${APP_ICON_RC}
)

target_link_libraries(OurPaintApp PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ourpaint_ui
)

install(TARGETS OurPaintApp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)