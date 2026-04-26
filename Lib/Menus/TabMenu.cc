#include "TabMenu.h"

#include <QEvent>


UI::TabMenu::TabMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    setObjectName(QStringLiteral("TabMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    renameTab_ = addAction("");
    deleteTab_ = addAction("");
    closeTab_ = addAction("");

    addSeparator();

    closeAllLeftTabs_ = addAction("");
    closeAllRightTabs_ = addAction("");
    closeAllTabs_ = addAction("");

    connect(deleteTab_, &QAction::triggered,
            this, &UI::TabMenu::deleteTabTriggered);

    connect(renameTab_, &QAction::triggered,
            this, &UI::TabMenu::renameTabTriggered);

    connect(closeTab_, &QAction::triggered,
            this, &UI::TabMenu::closeTabTriggered);

    connect(closeAllTabs_, &QAction::triggered,
            this, &UI::TabMenu::closeAllTabsTriggered);
    connect(closeAllLeftTabs_, &QAction::triggered,
            this, &UI::TabMenu::closeAllLeftTabsTriggered);
    connect(closeAllRightTabs_, &QAction::triggered,
            this, &UI::TabMenu::closeAllRightTabsTriggered);

    translate();
}


void UI::TabMenu::translate() const {
    if (renameTab_) {
        renameTab_->setText(tr("Rename tab"));
    }
    if (deleteTab_) {
        deleteTab_->setText(tr("Delete file"));
    }
    if (closeTab_) {
        closeTab_->setText(tr("Close tab"));
    }
    if (closeAllLeftTabs_) {
        closeAllLeftTabs_->setText(tr("Close all left tabs"));
    }
    if (closeAllRightTabs_) {
        closeAllRightTabs_->setText(tr("Close all right tabs"));
    }
    if (closeAllTabs_) {
        closeAllTabs_->setText(tr("Close all tabs"));
    }
}
