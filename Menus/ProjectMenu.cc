#include "ProjectMenu.h"


UI::ProjectMenu::ProjectMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    createProject_ = addAction(QIcon(":/Assets/icons/menus/projectCreate.ico"), "Create new project");
    openProject_ = addAction(QIcon(":/Assets/icons/menus/projectOpen.ico"), "Open project");

    addSeparator();

    createFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "Create new file");
    openFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "Open file");
    exportFile_ = addAction("Export file to...");

    addSeparator();

    script_ = addAction("Script");

    setObjectName(QStringLiteral("ProjectMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    connect(createProject_, &QAction::triggered, this, &ProjectMenu::createProjectTriggered);
    connect(openProject_, &QAction::triggered, this, &ProjectMenu::openProjectTriggered);
    connect(createFile_, &QAction::triggered, this, &ProjectMenu::createFileTriggered);
    connect(openFile_, &QAction::triggered, this, &ProjectMenu::openFileTriggered);
    connect(exportFile_, &QAction::triggered, this, &ProjectMenu::exportFileTriggered);
    connect(script_, &QAction::triggered, this, &ProjectMenu::scriptTriggered);
}
