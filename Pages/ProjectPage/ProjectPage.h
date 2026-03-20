#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QWidget>

class QLineEdit;
class QStackedWidget;
class QOpenGLWindow;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace UI {
    class TopBarProject;
    class TabBar;
    class ToolBar;
    class PainterWidget;
    class CommandConsole;
    class SideMenu;
    class ContainerWidget;
    class NavigationWidget;


    class ProjectPage final : public QWidget {
        Q_OBJECT

    public:
        explicit ProjectPage(QWidget* parent = nullptr);
        ~ProjectPage() override = default;
        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;
        void setDefaultProjectsPath(const QString& projectPath);
    signals:
        void openNewWindowCreateProjectTriggered(const QString& projectName);
        void createProjectThisWindowTriggered(const QString& projectName);

        void openNewWindowOpenProjectTriggered(const QString& projectName);
        void openProjectThisWindowTriggered(const QString& projectName);
        void openFileTriggered(const QString& filePath); // (topBar)

        void crateFileTriggered(const QString& fileName);

        /// Tabs
        void setActiveTabTriggered(const QString& name);
        void removeTabTriggered(const QString& name);
        void renameTabTriggered(const QString& oldName, const QString& newName);
        void openTabWindowTriggered(const QString& fileName);

        /// Navigation
        void goToStartWindowTriggered();
        void createFileTriggered(const QString& fileName); // (tab/top/navigation)
        void createProjectTriggered();
        void closeApplicationTriggered();

        /// Tools - constrains
        void pointLineDistanceTriggered();
        void pointOnLineTriggered();
        void pointPointDistanceTriggered();
        void coincidentPointsTriggered();
        void lineCircleDistanceTriggered();
        void lineOnCircleTriggered();
        void lineInCircleTriggered();
        void parallelLinesTriggered();
        void perpendicularLinesTriggered();
        void angleBetweenLinesTriggered();

        /// Tools - point
        void pointTriggered();

        /// Tools - line
        void lineTriggered();
        void polylineTriggered();
        void infiniteLineTriggered();

        /// Tools - circle
        void circleByDiameterTriggered();
        void circleTwoPointsTriggered();
        void ellipseThreePointsTriggered();

        /// Tools - arc
        void arcByRadiusTriggered();
        void arcByDiameterTriggered();
        void arcByThreePointsTriggered();

        /// Tools - settings
        void lineSettingsTriggered();
        void circleSettingsTriggered();
        void arcSettingsTriggered();

        /// Tools - tool
        void cursorTriggered();
        void sizeTriggered();

        /// TopBar - Project
        void openProjectTriggered();
        void exportFileTriggered();
        void scriptTriggered();
        // void openFileTriggered();
        // void createProjectTriggered(); (Navigation)
        // void createFileTriggered(); (Navigation/TopBar/TabBar)

        /// TopBar - Collaboration
        void collaborationOpenTriggered();
        void collaborationCloseTriggered();

        /// TopBar - Version Control
        void versionInitTriggered();
        void versionCreateTriggered();
        void versionPushTriggered();
        void versionPullTriggered();
        void versionCommitTriggered();

        /// Console Command
        void sentCommandTriggered(const QString& command);

    private:
        void initUI();
        void setupConnections();

        void createLayouts();
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

    public slots:
        void onAddTabSlot(const QString& name) const;
        void onDeleteTabSlot(const QString& name) const;
        void onRenameTabSlot(const QString& oldName, const QString& newName) const;

    private:
        QVBoxLayout* mainLayout_{nullptr};
        QHBoxLayout* rootLayout_{nullptr};
        QVBoxLayout* centerLayout_{nullptr};

        QVBoxLayout* workspaceLayout_{nullptr};
        QVBoxLayout* painterLayout_{nullptr};
        QHBoxLayout* consoleLayout_{nullptr};

        QWidget* mainArea_{nullptr};
        QWidget* centerColumn_{nullptr};

        QStackedWidget* centerStack_{nullptr};

        QWidget* workspacePage_{nullptr};
        NavigationWidget* navigationPage_{nullptr};

        QWidget* painterWrapper_{nullptr};
        QWidget* consoleWrapper_{nullptr};

        TopBarProject* topBar_{nullptr};
        TabBar* tabBar_{nullptr};
        ToolBar* toolBar_{nullptr};

        PainterWidget* painter_{nullptr};
        CommandConsole* console_{nullptr};

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
