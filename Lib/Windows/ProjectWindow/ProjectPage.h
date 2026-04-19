#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QString>

#include "../BaseEditorPage.h"

class QPushButton;
class QStackedWidget;

namespace UI {
    class ContainerWidget;
    class NavigationWidget;
    class SideMenu;
    class TabBar;
    class TopBarProject;


    class ProjectPage final : public BaseEditorPage {
        Q_OBJECT

    public:
        explicit ProjectPage(QWidget* parent = nullptr,const QString& name = "");
        ~ProjectPage() override = default;

        void setDefaultProjectsPath(const QString& projectPath);

    signals:
        void openNewWindowCreateProjectTriggered(const QString& projectPath);
        void createProjectThisWindowTriggered(const QString& projectPath);
        void openNewWindowOpenProjectTriggered(const QString& projectPath);
        void openProjectThisWindowTriggered(const QString& projectPath);
        void openFileTriggered(const QString& filePath);

        void crateFileTriggered(const QString& fileName);
        void setActiveTabTriggered(const QString& name);
        void removeTabTriggered(const QString& name);
        void renameTabTriggered(const QString& oldName, const QString& newName);
        void openTabWindowTriggered(const QString& fileName);
        void tabMovedToAnotherWindowTriggered(const QString& tabName);


        void goToStartWindowTriggered();
        void createFileTriggered(const QString& fileName);
        void createProjectTriggered();
        void closeApplicationTriggered();

        void openProjectTriggered();
        void exportFileTriggered();
        void scriptTriggered(const QString& text);

        void collaborationOpenTriggered();
        void collaborationCloseTriggered();

        void versionInitTriggered();
        void versionCreateTriggered();
        void versionPushTriggered();
        void versionPullTriggered();
        void versionCommitTriggered();

      void applySettings(double value1, double value2, double value3);

    private:
        void initUI() override;
        void setupConnections();

        void createTopBar();
        void createSideColumns();
        void createCenterColumn();
        void createWorkspace();
        void createNavigation();
        void createSideContainers();
        void assembleLayout() const;

    private slots:
        void createFileSlot();
        void updatePageSlot() const;
        void openInformationPanel() const;
        void openMessengerPanel() const;
        void onCreateProjectSlot();
        void onOpenProjectSlot();
        void openFileSlot();
        void onOpenScriptSlot();

    public slots:
        void onAddTabSlot(const QString& name) const;
        void onDeleteTabSlot(const QString& name) const;
        void onRenameTabSlot(const QString& oldName, const QString& newName) const;

    private:
        QStackedWidget* centerStack_{nullptr};
        QWidget* workspacePage_{nullptr};
        NavigationWidget* navigationPage_{nullptr};

        TopBarProject* topBar_{nullptr};
        TabBar* tabBar_{nullptr};
        SideMenu* leftColumn_{nullptr};
        SideMenu* rightColumn_{nullptr};

        ContainerWidget* messengerContainer_{nullptr};
        ContainerWidget* infoContainer_{nullptr};
        QPushButton* messengerButton_{nullptr};
        QPushButton* infoButton_{nullptr};

        QString projectDefaultPath_;
    };
} // namespace UI

#endif
