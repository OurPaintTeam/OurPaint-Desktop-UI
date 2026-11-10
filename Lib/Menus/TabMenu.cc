#include "TabMenu.h"

#include <QEvent>

#include "MenuTypes.h"


UI::TabMenu::TabMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    setObjectName(QStringLiteral("TabMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    renameTab_ = addAction("");
    deleteTab_ = addAction("");
    closeTab_ = addAction("");

    addSeparator();

    closeOtherTabs_ = addAction("");
    closeAllLeftTabs_ = addAction("");
    closeAllRightTabs_ = addAction("");
    closeAllTabs_ = addAction("");

    connect(deleteTab_, &QAction::triggered, this, [this] {
    emit tabMenuTriggered(UI::tabMenuType::deleteTab);
});
    connect(renameTab_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::renameTab);
    });
    connect(closeTab_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::closeTab);
    });
    connect(closeAllTabs_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::closeAllTabs);
    });
    connect(closeAllLeftTabs_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::closeAllLeftTabs);
    });
    connect(closeAllRightTabs_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::closeAllRightTabs);
    });
    connect(closeOtherTabs_, &QAction::triggered, this, [this] {
        emit tabMenuTriggered(UI::tabMenuType::closeOtherTabs);
    });

    translate();
}


void UI::TabMenu::translate() const {
    if (renameTab_) {
        renameTab_->setText(UI::TabMenu::tr("Rename tab"));
    }
    if (deleteTab_) {
        deleteTab_->setText(UI::TabMenu::tr("Delete file"));
    }
    if (closeTab_) {
        closeTab_->setText(UI::TabMenu::tr("Close file"));
    }
    if (closeAllLeftTabs_) {
        closeAllLeftTabs_->setText(UI::TabMenu::tr("Close all left tabs"));
    }
    if (closeAllRightTabs_) {
        closeAllRightTabs_->setText(UI::TabMenu::tr("Close all right tabs"));
    }
    if (closeAllTabs_) {
        closeAllTabs_->setText(UI::TabMenu::tr("Close all tabs"));
    }
    if (closeOtherTabs_) {
        closeOtherTabs_->setText(UI::TabMenu::tr("Close other tabs"));
    }
}
