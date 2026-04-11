#ifndef OURPAINT_DESKTOP_UI_UIMANAGER_H
#define OURPAINT_DESKTOP_UI_UIMANAGER_H

#include "FileSystem.h"
#include "MainWindow.h"

namespace UI {
    class UIManager final : public QObject {
        Q_OBJECT

    public:
        UIManager();
        ~UIManager() override = default;

    private:
        QVector<UI::MainWindow*> mainWindows_;
        UI::FileSystem fs_;

        bool checkedOpened(const QString& projectPath) const;
        void sentCommandFromConsole(const UI::MainWindow* window, const QString& command);
        void closeApplication(const UI::MainWindow* window, const QString& projectPath);
        void openNewWindowOpenProjectSlot(const UI::MainWindow* window, const QString& projectPath);
        void openNewWindowCreateProjectSlot(const UI::MainWindow* window, const QString& projectPath);
        void openProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const;
        void createProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const;
        void openFileSlot(const UI::MainWindow* window, const QString& filePath) const;
        void renameTabSlot(const UI::MainWindow* window, const QString& oldName, const QString& newName) const;
        void removeTabSlot(const UI::MainWindow* window, const QString& tabName) const;
        void createFileSlot(const UI::MainWindow* window, const QString& fileName) const;
        void goToStartWindowSlot(UI::MainWindow* window) const;
        void renameProjectSlot(UI::MainWindow* window, const QString& newName, const QString& path) const;
        void deleteProjectSlot(UI::MainWindow* window, const QString& path) const;
        void deleteSlot(UI::MainWindow* window);

        void primitiveSlot(const UI::MainWindow* window, PrimitiveType& type);
        void constraintSlot(const UI::MainWindow* window, ConstraintType& type);
        void toolsSlot(const UI::MainWindow* window, ToolsType& type);

        UI::MainWindow* createWindow(UI::MainWindow* parent, const UI::MainWindow::ProjectData& projectData);
        void initSignals(UI::MainWindow& window);
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_UIMANAGER_H
