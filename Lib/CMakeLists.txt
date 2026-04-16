cmake_minimum_required(VERSION 3.16)
project(OurPaint-Desktop-UI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGLWidgets OpenGL)

set(CORE
        Core/ProjectManager.cc
        Core/ProjectManager.cc
        Core/ProjectManager.cc
        Core/ProjectManager.h
        Core/ToolsType.h
)

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
        Widgets/PainterWidget.cc
        Widgets/PainterWidget.h
        Widgets/CommandConsole.cc
        Widgets/CommandConsole.h
)

set(PANELS
        Panels/MessengerPanel.cc
        Panels/MessengerPanel.h
        Panels/InformationPanel.cc
        Panels/InformationPanel.h
)

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

set(DRAG
        DragDrop/RotatedLabel.cc
        DragDrop/RotatedLabel.h
        DragDrop/DraggleButton.cc
        DragDrop/DraggleButton.h
        DragDrop/SlotWidget.cc
        DragDrop/SlotWidget.h
        DragDrop/ContainerWidget.cc
)

set(WINDOWS
        Windows/BaseEditorTopBar.cc
        Windows/BaseEditorTopBar.h
        Windows/BaseWindow.cc
        Windows/BaseWindow.h
        Windows/BaseEditorWindow.cc
        Windows/BaseEditorWindow.h
        Windows/FramelessWindow.cc
        Windows/FramelessWindow.h
        Windows/BaseEditorPage.cc
        Windows/BaseEditorPage.h
        Windows/BasePage.cc
        Windows/BasePage.h
)

set(UI_START
        Windows/StartWindow/StartWindow.cc
        Windows/StartWindow/StartWindow.h
        Windows/StartWindow/StartPage.cc
        Windows/StartWindow/StartPage.h
        Windows/StartWindow/TopBarStart.cc
        Windows/StartWindow/TopBarStart.h
        Windows/StartWindow/AnimationArea.cc
        Windows/StartWindow/AnimationArea.h
        Windows/StartWindow/ProjectsListWidget.cc
        Windows/StartWindow/ProjectsListWidget.h
        Windows/StartWindow/AnimationWidget.cc
        Windows/StartWindow/AnimationWidget.h
        Windows/StartWindow/ProjectButton.cc
        Windows/StartWindow/ProjectButton.h
)

set(UI_PROJECT
        Windows/ProjectWindow/ProjectWindow.cc
        Windows/ProjectWindow/ProjectWindow.h
        Windows/ProjectWindow/ProjectPage.cc
        Windows/ProjectWindow/ProjectPage.h
        Windows/ProjectWindow/TopBarProject.cc
        Windows/ProjectWindow/TopBarProject.h
        Windows/ProjectWindow/TabWidget.cc
        Windows/ProjectWindow/TabWidget.h
        Windows/ProjectWindow/TabBar.cc
        Windows/ProjectWindow/TabBar.h
        Windows/ProjectWindow/NavigationWidget.cc
)

set(UI_TAB
        Windows/TabWindow/TabWindow.cc
        Windows/TabWindow/TabWindow.h
        Windows/TabWindow/TabPage.cc
        Windows/TabWindow/TabPage.h
        Windows/TabWindow/TopBarTab.cc
        Windows/TabWindow/TopBarTab.h
)

set(POP_UP
        PopUp/InputWidget.cc
        PopUp/InputWidget.h
        PopUp/ParameterInputWidget.cc
        PopUp/ParameterInputWidget.h
        PopUp/NameInputWidget.cc
        PopUp/NameInputWidget.h
        PopUp/PathInputWidget.cc
        PopUp/PathInputWidget.h
)

set(FILE_SYSTEM
        FileSystem/FileSystem.cc
        FileSystem/FileSystem.h
)

add_library(ourpaint_ui STATIC
        resources.qrc
        ${CORE}
        ${WIDGETS}
        ${PANELS}
        ${MENUS}
        ${DRAG}
        ${WINDOWS}
        ${UI_START}
        ${UI_PROJECT}
        ${UI_TAB}
        ${POP_UP}
        ${FILE_SYSTEM}
)

target_include_directories(ourpaint_ui PUBLIC
        ..
        Styles
        Assets
        Core
        Widgets
        Panels
        Menus
        DragDrop
        PopUp
        FileSystem
        Windows
        Windows/StartWindow
        Windows/ProjectWindow
        Windows/TabWindow
)

target_link_libraries(ourpaint_ui PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGLWidgets
        Qt6::OpenGL
)


if (WIN32)
    target_link_libraries(ourpaint_ui PUBLIC dwmapi)
endif()