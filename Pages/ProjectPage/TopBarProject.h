#ifndef OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
#define OURPAINT_DESKTOP_UI_TOPBARPROJECT_H

#include "TopBar.h"

class TabBar;
class SideMenu;
class QPushButton;

// Top bar for Project workspace
class TopBarProject final : public TopBar
{
    Q_OBJECT

public:
    explicit TopBarProject(QWidget* parent = nullptr);
    ~TopBarProject() override = default;

    void setTabBar(TabBar* bar);
    void setLeftMenu(SideMenu* menu);
    void setRightMenu(SideMenu* menu);

private:
    // Pointers to linked UI components
    TabBar* tabBar      {nullptr};
    SideMenu* leftMenu  {nullptr};
    SideMenu* rightMenu {nullptr};

    // Buttons
    QPushButton* btnTabs     {nullptr};
    QPushButton* btnLeft     {nullptr};
    QPushButton* btnRight    {nullptr};
    QPushButton* btnSettings {nullptr};

    void createProjectButtons();
};

#endif // OURPAINT_DESKTOP_UI_TOPBARPROJECT_H