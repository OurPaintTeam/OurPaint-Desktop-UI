#ifndef OURPAINT_DESKTOP_UI_MAINWINDOW_H
#define OURPAINT_DESKTOP_UI_MAINWINDOW_H

#include "BaseEditorWindow.h"
#include "Utils.h"
#include <QStackedWidget>

class QCloseEvent;

namespace UI {
    class StartPage;
    class EditorPage;

    class MainWindow final : public BaseEditorWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override = default;

        void initUI() override;
        void setNotificationsEnabled(bool enable);
        void setFilesList(const QVector<ProjectData>& files) const;
        void setDefaultFilesPath(const QString& path) const;
        void setDefaultProjectPath(const QString& path);
        void setDefaultScriptPath(const QString& path);
        void addTabSlot(const FileData& data) const;
        void closeTabSlot(qint64 id) const;
        void deleteTabSlot(qint64 id) const;
        void renameTabSlot(FileData& data) const;
        void showStartPage();
        void showEditorPage();
        qint64 getActiveTabId() const;

        void closeApplicationSlot();

        QStackedWidget* stack() const;
        StartPage* startPage() const;
        EditorPage* editorPage() const;

    signals:
        void closeApplicationTriggered();
        void goToStartWindowTriggered();
        void openFileByIDTriggered(qint64 id, bool newWindow);
        void openFileTriggered(const QString& path, bool newWindow);
        void createFileTriggered(const QString& path, bool newWindow);
        void tabMovedToAnotherWindow(qint64 fileId, quintptr targetWindowPtr);
        void renameFileTriggered(FileData& data);
        void deleteFileTriggered(qint64 id);
        void renameTabTriggered(FileData& data);
        void removeTabTriggered(qint64 id);
        void setActiveTabTriggered(qint64 id);
        void closeTabTriggered(qint64 id);
        void openLightEditorWindowTriggered(qint64 id);
        void onOpenScriptTriggered(const QString& path);
        void settingsChanged();

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        QStackedWidget* stack_{nullptr};
        StartPage* startPage_{nullptr};
        EditorPage* editorPageWidget_{nullptr};
    };
}

#endif
