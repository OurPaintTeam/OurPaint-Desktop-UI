#ifndef OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
#define OURPAINT_DESKTOP_UI_TOPBARPROJECT_H


#include "TopBar.h"

#include <QPushButton>

namespace UI {
    class TabBar;
    class SideMenu;
    // Top bar for Project workspace
    class TopBarProject final : public TopBar {
        Q_OBJECT

    public:
        explicit TopBarProject(QWidget* parent = nullptr);
        ~TopBarProject() override = default;

        void setTabBar(TabBar* bar);
        void setLeftMenu(SideMenu* menu);
        void setRightMenu(SideMenu* menu);

    private:
        // Pointers to linked UI components
        TabBar* tabBar_      {nullptr};
        SideMenu* leftMenu_  {nullptr};
        SideMenu* rightMenu_ {nullptr};

        // Buttons
        QPushButton* btnTabs_     {nullptr};
        QPushButton* btnLeft_     {nullptr};
        QPushButton* btnRight_    {nullptr};
        QPushButton* btnSettings_ {nullptr};

        void createProjectButtons();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
