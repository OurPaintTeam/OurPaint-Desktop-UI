#include "ProjectMenu.h"

#include <QEvent>


UI::ProjectMenu::ProjectMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    createProject_ = addAction(QIcon(":/Assets/icons/menus/projectCreate.ico"), "");
    openProject_ = addAction(QIcon(":/Assets/icons/menus/projectOpen.ico"), "");

    addSeparator();

    createFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "");
    openFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "");
    exportFile_ = addAction("Export file to...");

    addSeparator();

    script_ = addAction("");

    setObjectName(QStringLiteral("ProjectMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    connect(createProject_, &QAction::triggered, this, &ProjectMenu::createProjectTriggered);
    connect(openProject_, &QAction::triggered, this, &ProjectMenu::openProjectTriggered);
    connect(createFile_, &QAction::triggered, this, &ProjectMenu::createFileTriggered);
    connect(openFile_, &QAction::triggered, this, &ProjectMenu::openFileTriggered);
    connect(exportFile_, &QAction::triggered, this, &ProjectMenu::exportFileTriggered);
    connect(script_, &QAction::triggered, this, &ProjectMenu::scriptTriggered);

    translate();
}


void UI::ProjectMenu::translate() const {
    if (createProject_) {
        createProject_->setText(UI::ProjectMenu::tr("Create new project"));
    }
    if (openProject_) {
        openProject_->setText(UI::ProjectMenu::tr("Open project"));
    }
    if (createFile_) {
        createFile_->setText(UI::ProjectMenu::tr("Create new file"));
    }
    if (openFile_) {
        openFile_->setText(UI::ProjectMenu::tr("Open file"));
    }
    if (exportFile_) {
        exportFile_->setText(UI::ProjectMenu::tr("Export file to..."));
    }
    if (script_) {
        script_->setText(UI::ProjectMenu::tr("Script"));
    }
}
