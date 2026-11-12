cmake_minimum_required(VERSION 3.16)
project(OurPaint-Desktop-UI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets OpenGL Svg Sql Network)
qt_standard_project_setup()

set(CORE
        Core/ProjectManager.cc
        Core/ProjectManager.h
        Core/ToolsType.h
        Core/Utils.h
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
        Widgets/NotificationManager.cc
        Widgets/NotificationManager.h
        Widgets/NotificationWidget.cc
        Widgets/NotificationWidget.h
        Widgets/PainterWidget.cc
        Widgets/PainterWidget.h
        Widgets/CommandConsole.cc
        Widgets/CommandConsole.h
        Widgets/RenameTabLineEdit.cc
        Widgets/RenameTabLineEdit.h
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
        Menus/FileMenu.cc
        Menus/FileMenu.h
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
        Menus/TabMenu.cc
        Menus/TabMenu.h
        Menus/MenuTypes.h
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
        Windows/MainWindow.cc
        Windows/MainWindow.h
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
        Windows/StartWindow/FilesListWidget.cc
        Windows/StartWindow/FilesListWidget.h
        Windows/StartWindow/AnimationWidget.cc
        Windows/StartWindow/AnimationWidget.h
        Windows/StartWindow/FileButton.cc
        Windows/StartWindow/FileButton.h
)

set(UI_EDITOR
        Windows/EditorWindow/EditorWindow.cc
        Windows/EditorWindow/EditorWindow.h
        Windows/EditorWindow/EditorPage.cc
        Windows/EditorWindow/EditorPage.h
        Windows/EditorWindow/TopBarEditor.cc
        Windows/EditorWindow/TopBarEditor.h
        Windows/EditorWindow/TabWidget.cc
        Windows/EditorWindow/TabWidget.h
        Windows/EditorWindow/TabBar.cc
        Windows/EditorWindow/TabBar.h
        Windows/EditorWindow/NavigationWidget.cc
)

set(UI_LIGHT_EDITOR
        Windows/LightEditorWindow/LightEditorWindow.cc
        Windows/LightEditorWindow/LightEditorWindow.h
        Windows/LightEditorWindow/LightEditorPage.cc
        Windows/LightEditorWindow/LightEditorPage.h
        Windows/LightEditorWindow/TopBarLightEditor.cc
        Windows/LightEditorWindow/TopBarLightEditor.h
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
        PopUp/ConfirmDialog.cc
        PopUp/ConfirmDialog.h
)

set(DOCUMENT_SYSTEMS
        DocumentSystems/FileSystem.cc
        DocumentSystems/FileSystem.h
        DocumentSystems/SettingsSystem.cc
        DocumentSystems/SettingsSystem.h
)

qt_add_library(ourpaint_ui STATIC
        resources.qrc
        translations.qrc
        ${CORE}
        ${WIDGETS}
        ${PANELS}
        ${MENUS}
        ${DRAG}
        ${WINDOWS}
        ${UI_START}
        ${UI_EDITOR}
        ${UI_LIGHT_EDITOR}
        ${POP_UP}
        ${DOCUMENT_SYSTEMS}
)

target_include_directories(ourpaint_ui PUBLIC
        ..
        Core
        Widgets
        Panels
        Menus
        DragDrop
        PopUp
        DocumentSystems
        Windows
        Windows/StartWindow
        Windows/EditorWindow
        Windows/LightEditorWindow
)

target_link_libraries(ourpaint_ui PUBLIC
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::Sql
        Qt6::Svg
        Qt6::Network
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(ourpaint_ui PUBLIC )
endif()

if (WIN32)
    qt_import_plugins(ourpaint_ui
            INCLUDE_BY_TYPE imageformats Qt6::QSvgPlugin
    )

    target_link_libraries(ourpaint_ui PUBLIC dwmapi)
endif()

