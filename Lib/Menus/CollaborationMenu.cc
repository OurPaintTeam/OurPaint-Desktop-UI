#include "CollaborationMenu.h"

#include <QEvent>


UI::CollaborationMenu::CollaborationMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    setObjectName("collaborationMenu");

    open_ = addAction("");
    addSeparator();
    close_ = addAction("");

    setObjectName(QStringLiteral("CollaborationMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    connect(open_, &QAction::triggered, this, &CollaborationMenu::openTriggered);
    connect(close_, &QAction::triggered, this, &CollaborationMenu::closeTriggered);

    translate();
}


void UI::CollaborationMenu::translate() const {
    if (open_) {
        open_->setText(UI::CollaborationMenu::tr("Open"));
    }
    if (close_) {
        close_->setText(UI::CollaborationMenu::tr("Close"));
    }
}
