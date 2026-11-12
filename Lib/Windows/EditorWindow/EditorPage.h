#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QString>

#include "MenuTypes.h"
#include "Utils.h"
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
        explicit EditorPage(QWidget* parent = nullptr);
        ~EditorPage() override = default;

        void setDefaultProjectPath(const QString& path);
        void setDefaultScriptPath(const QString& path);
        qint64 getActiveTabId() const;

    signals:
        void createFileTriggered(const QString& path,bool newWindow);
        void openFileTriggered(const QString& path,bool newWindow);

        void setActiveTabTriggered(const qint64 id);
        void removeTabTriggered(const qint64 id);
        void renameTabTriggered(FileData& data);
        void closeTabTriggered(const qint64 id);
        void tabMovedToAnotherWindow(qint64 fileId, quintptr sourceWindowPtr, quintptr targetWindowPtr);

        void openLightEditorWindowTriggered(const qint64 id);

        void goToStartWindowTriggered();
        void closeApplicationTriggered();

        void exportFileTriggered();
        void scriptTriggered(const QString& path);

        void collaborationTriggered(UI::collab с);
        void versionTriggered(UI::version v);

        void settingsChanged();


    protected:
        void changeEvent(QEvent *e) override;
        void setSettings(SettingsSystem* settings) const override;

    private:
        void initUI() override;
        void setupConnections();
        void onProjectAction(UI::projectMenuType p);

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
        void onAddTabSlot(const FileData& data) const;
        void onCloseTabSlot(qint64 id) const;
        void onDeleteTabSlot(const qint64 id) const;
        void onRenameTabSlot(FileData& data) const;

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

        QString defaultProjectPath_;
        QString defaultScriptPath_;
    };
} // namespace UI

#endif
