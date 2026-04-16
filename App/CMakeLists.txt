cmake_minimum_required(VERSION 3.16)
project(OurPaintApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

add_executable(OurPaintApp
        main.cc
        CustomConsole.cc
        RenderEngine.cc
        UIManager.cc
)

target_link_libraries(OurPaintApp PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ourpaint_ui
)