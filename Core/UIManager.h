#ifndef OURPAINT_DESKTOP_UI_UIMANAGER_H
#define OURPAINT_DESKTOP_UI_UIMANAGER_H

#include "FileSystem.h"
#include "ProjectManager.h"

class QLineEdit;
class QWindow;

namespace UI {
    class UIManager final : public QObject {
        Q_OBJECT

    public:
        UIManager();
        ~UIManager() override = default;

    private:
        QVector<UI::ProjectManager*> projectManagers_;
        UI::FileSystem fs_;

        bool checkedOpened(const QString& projectPath) const;
        void sentCommandFromConsole(UI::ProjectManager* manager, const QString& command);
        void openNewWindowOpenProjectSlot(UI::ProjectManager* manager, const QString& projectPath);
        void openNewWindowCreateProjectSlot(UI::ProjectManager* manager, const QString& projectPath);
        void openProjectThisWindowSlot(UI::ProjectManager* manager, const QString& projectPath) const;
        void createProjectThisWindowSlot(UI::ProjectManager* manager, const QString& projectPath) const;
        void openTabWindowSlot(UI::ProjectManager* manager, const QString& nameTab) const;
        void openFileSlot(UI::ProjectManager* manager, const QString& filePath) const;
        void renameTabSlot(UI::ProjectManager* manager, const QString& oldName, const QString& newName) const;
        void removeTabSlot(UI::ProjectManager* manager, const QString& tabName) const;
        void createFileSlot(UI::ProjectManager* manager, const QString& fileName) const;
        void goToStartWindowSlot(UI::ProjectManager* manager) const;
        void renameProjectSlot(UI::ProjectManager* manager, const QString& newName, const QString& path) const;
        void deleteProjectSlot(UI::ProjectManager* manager, const QString& path) const;
        void deleteSlot(UI::ProjectManager* manager);

        void primitiveSlot(UI::ProjectManager* manager, PrimitiveType& type);
        void constraintSlot(UI::ProjectManager* manager, ConstraintType& type);
        void toolsSlot(UI::ProjectManager* manager, ToolsType& type);

        UI::ProjectManager* createProjectManager(const UI::ProjectManager::ProjectData& projectData = {});
        UI::ProjectManager* createOpenedProjectManager(const UI::ProjectManager::ProjectData& projectData,
                                                        QWindow* windowRender,
                                                        QLineEdit* consoleEngine);
        void initSignals(UI::ProjectManager& manager);
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_UIMANAGER_H
