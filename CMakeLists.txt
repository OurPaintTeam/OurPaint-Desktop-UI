cmake_minimum_required(VERSION 3.16)
project(OurPaint-Desktop-UI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGLWidgets OpenGL)



# ---------- Core ----------
set(CORE
        Core/MainWindow.cc
        Core/MainWindow.h
        Core/FramelessWindow.cc
        Core/FramelessWindow.h
        Core/ToolsType.h
)

set(COMMAND_CONSOLE
        CommandConsole/CommandConsole.cc
        CommandConsole/CommandConsole.h
        CommandConsole/CustomConsole.cc
        CommandConsole/CustomConsole.h
)

# ---------- Widgets ----------
set(WIDGETS
        Widgets/CentralWidget.cc
        Widgets/CentralWidget.h
        Widgets/ToolBar.cc
        Widgets/ToolBar.h
        Widgets/SideMenu.cc
        Widgets/SideMenu.h
        Widgets/SideTool.cc
        Widgets/SideTool.h
        Widgets/SettingsWidget.cc
        Widgets/SettingsWidget.h
        Widgets/TopBar.cc
        Widgets/TopBar.h
        Widgets/NotificationContainer.cc
        Widgets/NotificationContainer.h
        Widgets/NotificationWidget.cc
        Widgets/NotificationWidget.h
)

# ---------- Panels ----------
set(PANELS
        Panels/MessengerPanel.cc
        Panels/MessengerPanel.h
        Panels/InformationPanel.cc
        Panels/InformationPanel.h
)

# ---------- Menus ----------
set(MENUS
        Menus/MenuButton.cc
        Menus/MenuButton.h
        Menus/CustomMenu.cc
        Menus/CustomMenu.h
        Menus/ProjectMenu.cc
        Menus/ProjectMenu.h
        Menus/CollaborationMenu.cc
        Menus/CollaborationMenu.h
        Menus/VersionControlMenu.cc
        Menus/VersionControlMenu.h
        Menus/ArcMenu.cc
        Menus/ArcMenu.h
        Menus/CircleMenu.cc
        Menus/CircleMenu.h
        Menus/LineMenu.cc
        Menus/LineMenu.h
)

# ---------- Controls ----------
set(DRAG
        DragDrop/RotatedLabel.cc
        DragDrop/RotatedLabel.h
        DragDrop/DraggleButton.cc
        DragDrop/DraggleButton.h
        DragDrop/SlotWidget.cc
        DragDrop/SlotWidget.h
        DragDrop/ContainerWidget.cc
)

# ---------- UI ----------
set(UI_START
        Pages/StartPage/StartPage.cc
        Pages/StartPage/StartPage.h
        Pages/StartPage/TopBarStart.cc
        Pages/StartPage/TopBarStart.h
        Pages/StartPage/AnimationArea.cc
        Pages/StartPage/AnimationArea.h
        Pages/StartPage/ProjectsListWidget.cc
        Pages/StartPage/ProjectsListWidget.h
        Pages/StartPage/AnimationWidget.cc
        Pages/StartPage/AnimationWidget.h
        Pages/StartPage/ProjectButton.cc
        Pages/StartPage/ProjectButton.h
)

set(UI_PROJECT
        Pages/ProjectPage/ProjectPage.cc
        Pages/ProjectPage/ProjectPage.h
        Pages/ProjectPage/TopBarProject.cc
        Pages/ProjectPage/TopBarProject.h
        Pages/ProjectPage/TabWidget.cc
        Pages/ProjectPage/TabWidget.h
        Pages/ProjectPage/TabBar.cc
        Pages/ProjectPage/TabBar.h
        Pages/ProjectPage/NavigationWidget.cc
)

set(UI_TAB
        Pages/TabPage/TabPage.cc
        Pages/TabPage/TabPage.h
        Pages/TabPage/TopBarTab.cc
        Pages/TabPage/TopBarTab.h
)

set(POP_UP
        PopUp/InputWidget.cc
        PopUp/InputWidget.h
)

set(FILE_SYSTEM
        FileSystem/FileSystem.cc
        FileSystem/FileSystem.h
)

set(PAINTER
        Painter/PainterWidget.cc
        Painter/PainterWidget.h
)



set(Resources
        resources.qrc
        Core/UIManager.cc
        Core/UIManager.h
        CommandConsole/CustomConsole.cc
        CommandConsole/CustomConsole.h
        PopUp/ParameterInputWidget.cc
        PopUp/ParameterInputWidget.h
        PopUp/NameInputWidget.cc
        PopUp/NameInputWidget.h
        PopUp/PathInputWidget.cc
        PopUp/PathInputWidget.h
)

add_executable(OurPaint-Desktop-UI
        main.cc
        ${COMMAND_CONSOLE}
        ${PAINTER}
        ${CORE}
        ${WIDGETS}
        ${PANELS}
        ${MENUS}
        ${DRAG}
        ${UI_START}
        ${UI_PROJECT}
        ${UI_TAB}
        ${POP_UP}
        ${FILE_SYSTEM}
        ${Resources}
)

target_include_directories(OurPaint-Desktop-UI PRIVATE
        .
        Core
        CommandConsole
        Widgets
        Panels
        Menus
        DragDrop
        PopUp
        FileSystem
        Painter
        Painter/QOpenGL
        Pages/StartPage
        Pages/ProjectPage
        Pages/TabPage
)




target_link_libraries(OurPaint-Desktop-UI PRIVATE
        Qt6::Widgets
        dwmapi
        Qt6::Core
        Qt6::OpenGLWidgets
        Qt6::OpenGL
)