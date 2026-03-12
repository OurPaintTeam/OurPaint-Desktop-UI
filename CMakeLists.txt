cmake_minimum_required(VERSION 3.16)
project(OurPaint-Desktop-UI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ---------- Core ----------
set(CORE
        Core/MainWindow.cc
        Core/MainWindow.h
        Core/FramelessWindow.cc
        Core/FramelessWindow.h
)

# ---------- Widgets ----------
set(WIDGETS
        Widgets/CentralWidget.cc
        Widgets/CentralWidget.h
        Widgets/ToolBar.cc
        Widgets/ToolBar.h
        Widgets/PainterWidget.cc
        Widgets/PainterWidget.h
        Widgets/CommandConsole.cc
        Widgets/CommandConsole.h
        Widgets/TabBar.cc
        Widgets/TabBar.h
        Widgets/SideMenu.cc
        Widgets/SideMenu.h
        Widgets/SideTool.cc
        Widgets/SideTool.h
        Widgets/SettingsWidget.cc
        Widgets/SettingsWidget.h
        Widgets/TopBar.cc
        Widgets/TopBar.h
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
set(CONTROLS
        Controls/MenuButton.cc
        Controls/MenuButton.h
        Controls/CustomMenu.cc
        Controls/CustomMenu.h
        Controls/HideOpenPanelButton.cc
        Controls/HideOpenPanelButton.h
        Controls/TabWidget.cc
        Controls/TabWidget.h
        Controls/DraggleContainer.cc
        Controls/DraggleContainer.h
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
)

set(UI_PROJECT
        Pages/ProjectPage/ProjectPage.cc
        Pages/ProjectPage/ProjectPage.h
        Pages/ProjectPage/TopBarProject.cc
        Pages/ProjectPage/TopBarProject.h
)

set(UI_TAB
        Pages/TabPage/TabPage.cc
        Pages/TabPage/TabPage.h
        Pages/TabPage/TopBarTab.cc
        Pages/TabPage/TopBarTab.h
)
set(Resources
        resources.qrc
)
add_executable(OurPaint-Desktop-UI
        main.cc
        ${CORE}
        ${WIDGETS}
        ${PANELS}
        ${MENUS}
        ${CONTROLS}
        ${UI_START}
        ${UI_PROJECT}
        ${UI_TAB}
        ${Resources}
)

target_include_directories(OurPaint-Desktop-UI PRIVATE
        .
        Core
        Widgets
        Panels
        Menus
        Controls
        Pages/StartPage
        Pages/ProjectPage
        Pages/TabPage
)

target_link_libraries(OurPaint-Desktop-UI PRIVATE Qt6::Widgets dwmapi)