#ifndef OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
#define OURPAINT_DESKTOP_UI_TOPBARPROJECT_H


#include "TopBar.h"

class QPushButton;
class QToolButton;

#include "ProjectMenu.h"

namespace UI {
    class CommandConsole;
    class TabBar;
    class SideMenu;
    class CollaborationMenu;
    class MenuButton;
    class VersionControlMenu;


    // Top bar for Project workspace
    class TopBarProject final : public TopBar {
        Q_OBJECT

    public:
        explicit TopBarProject(QWidget* parent = nullptr);
        ~TopBarProject() override = default;

        void setTabBar(TabBar* bar);
        void setLeftMenu(SideMenu* menu);
        void setRightMenu(SideMenu* menu);
        void setConsole(CommandConsole* console);

        void setTabsButtonEnabled(bool enabled) const;
        void setLeftButtonEnabled(bool enabled) const;
        void setRightButtonEnabled(bool enabled) const;
        void setConsoleButtonEnabled(bool enabled) const;

        void setPanelsButtonsEnabled(bool enabled) const;

    signals:
        /// Project
        void createProjectTriggered();
        void openProjectTriggered();
        void createFileTriggered();
        void openFileTriggered();
        void exportFileTriggered();
        void scriptTriggered();

        /// Collaboration
        void collaborationOpenTriggered();
        void collaborationCloseTriggered();

        /// Version Control
        void versionInitTriggered();
        void versionCreateTriggered();
        void versionPushTriggered();
        void versionPullTriggered();
        void versionCommitTriggered();

    private:
        // Pointers to linked UI components
        TabBar* tabBar_{nullptr};
        SideMenu* leftMenu_{nullptr};
        SideMenu* rightMenu_{nullptr};
        CommandConsole* commandConsole_{nullptr};

        // Buttons
        QPushButton* btnTabs_{nullptr};
        QPushButton* btnLeft_{nullptr};
        QPushButton* btnRight_{nullptr};
        QPushButton* btnConsole_{nullptr};
        QPushButton* btnSettings_{nullptr};
        ProjectMenu* projectMenu_{nullptr};
        CollaborationMenu* collaborationMenu_{nullptr};
        VersionControlMenu* versionControlMenu_{nullptr};
        MenuButton* projectButton_{nullptr};
        MenuButton* collabButton_{nullptr};
        MenuButton* verButton_{nullptr};
        QPushButton* helpButton_{nullptr};

        void createProjectButtons();
        void setupSettings();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
