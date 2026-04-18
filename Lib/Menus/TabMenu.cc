#include "TabMenu.h"


UI::TabMenu::TabMenu(QWidget* parent)
    : UI::CustomMenu(parent) {

  setObjectName(QStringLiteral("TabMenu"));
  setAttribute(Qt::WA_StyledBackground, true);

  renameTab_ = addAction("Rename tab");
  deleteTab_ = addAction("Delete file");
  closeTab_ = addAction("Close tab");

  addSeparator();

  closeAllTabs_ = addAction("Close all tabs");

  connect(deleteTab_, &QAction::triggered,
          this, &UI::TabMenu::deleteTabTriggered);

  connect(renameTab_, &QAction::triggered,
          this, &UI::TabMenu::renameTabTriggered);

  connect(closeTab_, &QAction::triggered,
          this, &UI::TabMenu::closeTabTriggered);

  connect(closeAllTabs_, &QAction::triggered,
          this, &UI::TabMenu::closeAllTabsTriggered);
}