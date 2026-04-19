#ifndef OURPAINT_TABMENU_H
#define OURPAINT_TABMENU_H
#include "CustomMenu.h"
namespace UI {
class TabMenu final : public UI::CustomMenu {
  Q_OBJECT
public:
  explicit TabMenu(QWidget* parent = nullptr);

signals:
      void deleteTabTriggered();
  void renameTabTriggered();
  void closeTabTriggered();
  void closeAllTabsTriggered();
    void closeAllRightTabsTriggered();
    void closeAllLeftTabsTriggered();
private:
  QAction* deleteTab_{nullptr};
  QAction* renameTab_{nullptr};
  QAction* closeTab_{nullptr};
    QAction* closeAllLeftTabs_{nullptr};
    QAction* closeAllRightTabs_{nullptr};
  QAction* closeAllTabs_{nullptr};

};
} // namespace UI



#endif //OURPAINT_TABMENU_H
