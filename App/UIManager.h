#ifndef OURPAINT_DESKTOP_UI_UIMANAGER_H
#define OURPAINT_DESKTOP_UI_UIMANAGER_H

#include "FileSystem.h"
#include "ProjectManager.h"

class QLineEdit;
class QWindow;

    class UIManager final : public QObject {
        Q_OBJECT

    public:
        UIManager();
        ~UIManager() override = default;

    private:
        QVector<UI::ProjectManager*> projectManagers_;
        UI::FileSystem fs_;

        qint64 defaultLocationId() const;

        UI::ProjectManager* createProjectManager();
        UI::ProjectManager* createEditorProjectManager(QWindow* windowRender,
                                                       QLineEdit* consoleEngine);
        void deleteSlot(UI::ProjectManager* manager);
        void initSignals(UI::ProjectManager& manager);
        void refreshFiles(UI::ProjectManager* manager) const;

        void openFileInCurrentWindow(UI::ProjectManager* manager, qint64 fileId) const;
        void openFileByPathInCurrentWindow(UI::ProjectManager* manager, const QString& path) const;
        void openFileByPathInNewWindow(const QString& path);
        void createFileInCurrentWindow(UI::ProjectManager* manager, const QString& fileName) const;
        void createFileInNewWindow(const QString& fileName);
        void renameFile(qint64 fileId, const QString& newName) const;
        void deleteFile(UI::ProjectManager* manager, qint64 fileId) const;
        void openLightEditor(UI::ProjectManager* manager, const QString& fileName, qint64 fileId);
        void returnTabFromLight(UI::ProjectManager* manager, qint64 fileId) const;
        void setActiveTab(UI::ProjectManager* manager, qint64 fileId) const;
        void goToStartWindow(UI::ProjectManager* manager) const;

        void sentCommandFromConsole(UI::ProjectManager* manager, qint64 fileId,
                                    const QString& command) const;
        void primitiveSlot(UI::ProjectManager* manager, UI::PrimitiveType& type, qint64 fileId);
        void constraintSlot(UI::ProjectManager* manager, UI::ConstraintType& type, qint64 fileId);
        void toolsSlot(UI::ProjectManager* manager, UI::ToolsType& type, qint64 fileId);
        void scriptSlot(UI::ProjectManager* manager, const QString& path);
        void settingsSlot(UI::ProjectManager* manager, double f, double s, double t);
    };

#endif //OURPAINT_DESKTOP_UI_UIMANAGER_H