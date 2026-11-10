#ifndef OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
#define OURPAINT_DESKTOP_UI_TOPBARPROJECT_H

#include "MenuTypes.h"
#include "Utils.h"
#include "../BaseEditorTopBar.h"

class QPushButton;
class QToolButton;

namespace UI {
    class SettingsWidget;
    class SettingsSystem;
    class TabBar;
    class SideMenu;
    class CollaborationMenu;
    class MenuButton;
    class VersionControlMenu;
    class FileMenu;


    // Top bar for Project workspace
    class TopBarEditor final : public BaseEditorTopBar {
        Q_OBJECT

    public:
        explicit TopBarEditor(QWidget* parent = nullptr);
        ~TopBarEditor() override = default;

        void setTabBar(TabBar* bar);
        void setLeftMenu(SideMenu* menu);
        void setRightMenu(SideMenu* menu);
        void setConsole(QWidget* console);
        void setSettings(SettingsSystem* settings = nullptr);

        void setTabsButtonEnabled(bool enabled) const;
        void setLeftButtonEnabled(bool enabled) const;
        void setRightButtonEnabled(bool enabled) const;
        void setPanelsButtonsEnabled(bool enabled) const;

    signals:
        void projectTriggered(UI::projectMenuType p);
        void collaborationTriggered(UI::collab с);
        void versionTriggered(UI::version v);

        void settingsChanged();
    protected:
        void changeEvent(QEvent *e) override;

    private:
        // Pointers to linked UI components
        TabBar* tabBar_{nullptr};
        // Buttons
        QPushButton* btnTabs_{nullptr};
        QPushButton* btnSettings_{nullptr};
        FileMenu* projectMenu_{nullptr};
        CollaborationMenu* collaborationMenu_{nullptr};
        VersionControlMenu* versionControlMenu_{nullptr};
        MenuButton* projectButton_{nullptr};
        MenuButton* collabButton_{nullptr};
        MenuButton* verButton_{nullptr};
        QPushButton* helpButton_{nullptr};
        SettingsSystem* settingsSystem_{nullptr};
        SettingsWidget* settingsWidget_{nullptr};

        void createProjectButtons();
        void setupSettings();
        void translate() const;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARPROJECT_H
