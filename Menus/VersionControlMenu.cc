#include "VersionControlMenu.h"

UI::VersionControlMenu::VersionControlMenu(QWidget* parent)
    : CustomMenu(parent)
{
    setObjectName(QStringLiteral("versionControlMenu"));

    init_ = addAction("Init");
    create_ = addAction("Create");

    addSeparator();

    push_ = addAction("Push");
    pull_ = addAction("Pull");
    commit_ = addAction("Commit");
}