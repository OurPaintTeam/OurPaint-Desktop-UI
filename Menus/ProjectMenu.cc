#include "ProjectMenu.h"

UI::ProjectMenu::ProjectMenu(QWidget* parent)
    : UI::CustomMenu(parent)
{
    createProject_ = addAction("Create new project");
    openProject_ = addAction("Open project");

    addSeparator();

    createFile_ = addAction("Create new file");
    exportFile_ = addAction("Export file to...");

    addSeparator();

    script_ = addAction("Script");

    setObjectName(QStringLiteral("ProjectMenu"));
}