#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QString>

#include "../BaseEditorPage.h"

class QPushButton;
class QStackedWidget;

namespace UI {
    class SettingsSystem;
    class ContainerWidget;
    class NavigationWidget;
    class SideMenu;
    class TabBar;
    class TopBarEditor;


    class EditorPage final : public BaseEditorPage {
        Q_OBJECT

    public:
        explicit EditorPage(QWidget* parent = nullptr, const qint64 id = -1);
        ~EditorPage() override = default;

        void setDefaultPath(const QString& path);


    signals:
        void openFileThisWindowTriggered(const QString& projectPath);
        void createFileThisWindowTriggered(const QString& projectPath);
        void openNewWindowOpenFileTriggered(const QString& projectPath);
        void openNewWindowCreateFileTriggered(const QString& projectPath);
        void openFileTriggered(const QString& filePath);

        void crateFileTriggered(const QString& fileName);
        void setActiveTabTriggered(const qint64 id);
        void removeTabTriggered(const qint64 id);
        void renameTabTriggered(const qint64 id, const QString& newName);
        void openLightEditorWindowTriggered(const qint64 id);
        void tabMovedToAnotherWindowTriggered(const qint64 id);
        void closeTabTriggered(const qint64 id);

        void goToStartWindowTriggered();
        void createFileTriggered(const QString& fileName);
        void closeApplicationTriggered();
        void exportFileTriggered();
        void scriptTriggered(const QString& path);

        void collaborationOpenTriggered();
        void collaborationCloseTriggered();

        void versionInitTriggered();
        void versionCreateTriggered();
        void versionPushTriggered();
        void versionPullTriggered();
        void versionCommitTriggered();

        void settingsChanged();
    protected:
        void changeEvent(QEvent *e) override;
        void setSettings(SettingsSystem* settings) const override;

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
        void openFileSlot();
        void onOpenScriptSlot();

    public slots:
        void onAddTabSlot(const QString& name,const qint64 id) const;
        void onDeleteTabSlot(const qint64 id) const;
        void onRenameTabSlot(const qint64 id, const QString& newName) const;

    private:
        void translate() const;
        
        QStackedWidget* centerStack_{nullptr};
        QWidget* workspacePage_{nullptr};
        NavigationWidget* navigationPage_{nullptr};

        TopBarEditor* topBar_{nullptr};
        TabBar* tabBar_{nullptr};
        SideMenu* leftColumn_{nullptr};
        SideMenu* rightColumn_{nullptr};

        ContainerWidget* messengerContainer_{nullptr};
        ContainerWidget* infoContainer_{nullptr};
        QPushButton* messengerButton_{nullptr};
        QPushButton* infoButton_{nullptr};

        QString defaultPath_;
    };
} // namespace UI

#endif
