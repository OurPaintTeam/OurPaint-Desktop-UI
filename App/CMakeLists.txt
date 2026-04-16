cmake_minimum_required(VERSION 3.16)

project(OurPaintApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(OurPaintApp
        main.cc
        CustomConsole.cc
        RenderEngine.cc
        UIManager.cc
)

target_link_libraries(OurPaintApp PRIVATE ourpaint_ui)