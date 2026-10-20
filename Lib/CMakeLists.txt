include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.9
    GIT_SHALLOW TRUE)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

find_package(OpenGL REQUIRED)

add_library(OurPaintLib STATIC
        src/Event.h
        src/Events.cc
        src/Events.h
        src/OurPaint.h
    src/OurPaint.cc
    src/App.cc
    src/Fbo.cc
    src/Platform.h
    src/App.h
    src/Fbo.h
    src/OurPaintInternal.h
    src/PlatformWin32.cc
    src/PlatformLinux.cc)
add_library(OurPaint::Lib ALIAS OurPaintLib)

target_include_directories(OurPaintLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(OurPaintLib PUBLIC glfw OpenGL::GL)

# Dear ImGui is an implementation detail of the library.
add_library(OurPaintImGui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(OurPaintImGui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(OurPaintImGui PUBLIC glfw)
target_link_libraries(OurPaintLib PRIVATE OurPaintImGui)

if(WIN32)
    target_sources(OurPaintLib PRIVATE src/PlatformWin32.cc)
    target_compile_definitions(OurPaintLib PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN GLFW_EXPOSE_NATIVE_WIN32)
    target_link_libraries(OurPaintLib PRIVATE dwmapi user32)
elseif(APPLE)
    message(FATAL_ERROR "OurPaint currently supports Windows and Linux only.")
else()
    target_sources(OurPaintLib PRIVATE src/PlatformLinux.cc)
    target_link_libraries(OurPaintLib PRIVATE dl pthread)
endif()

if(MSVC)
    target_compile_options(OurPaintLib PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(OurPaintLib PRIVATE -Wall -Wextra -Wpedantic)
endif()
