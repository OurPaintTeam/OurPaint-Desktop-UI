#include "VersionControlMenu.h"


UI::VersionControlMenu::VersionControlMenu(QWidget* parent)
    : CustomMenu(parent) {
    setObjectName(QStringLiteral("VersionControlMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    init_ = addAction("Init");
    create_ = addAction("Create");

    addSeparator();

    push_ = addAction("Push");
    pull_ = addAction("Pull");
    commit_ = addAction("Commit");

    connect(init_, &QAction::triggered, this, &VersionControlMenu::initTriggered);
    connect(create_, &QAction::triggered, this, &VersionControlMenu::createTriggered);
    connect(push_, &QAction::triggered, this, &VersionControlMenu::pushTriggered);
    connect(pull_, &QAction::triggered, this, &VersionControlMenu::pullTriggered);
    connect(commit_, &QAction::triggered, this, &VersionControlMenu::commitTriggered);
}
