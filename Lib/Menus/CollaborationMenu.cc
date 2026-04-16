#include "CollaborationMenu.h"


UI::CollaborationMenu::CollaborationMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    setObjectName("collaborationMenu");

    open_ = addAction("Open");
    addSeparator();
    close_ = addAction("Close");

    setObjectName(QStringLiteral("CollaborationMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    connect(open_, &QAction::triggered, this, &CollaborationMenu::openTriggered);
    connect(close_, &QAction::triggered, this, &CollaborationMenu::closeTriggered);
}
