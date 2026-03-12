#include "CollaborationMenu.h"

UI::CollaborationMenu::CollaborationMenu(QWidget* parent)
    : UI::CustomMenu(parent)
{
    setObjectName("collaborationMenu");

    open_ = addAction("Open");
    addSeparator();
    close_ = addAction("Close");

    setObjectName(QStringLiteral("CollaborationMenu"));
}