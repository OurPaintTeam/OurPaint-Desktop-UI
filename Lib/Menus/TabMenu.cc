#include "TabMenu.h"


UI::TabMenu::TabMenu(QWidget* parent)
    : UI::CustomMenu(parent) {

  setObjectName(QStringLiteral("TabMenu"));
  setAttribute(Qt::WA_StyledBackground, true);

  renameTab_ = addAction("Rename tab");
  deleteTab_ = addAction("Delete file");
  closeTab_ = addAction("Close tab");

  addSeparator();

    closeAllLeftTabs_ = addAction("Close all left tabs");
    closeAllRightTabs_ = addAction("Close all right tabs");
  closeAllTabs_ = addAction("Close all tabs");

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
}