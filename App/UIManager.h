#ifndef OURPAINT_DESKTOP_UI_UIMANAGER_H
#define OURPAINT_DESKTOP_UI_UIMANAGER_H

#include "FileSystem.h"
#include "ProjectManager.h"

class QLineEdit;
class QWindow;
class QTranslator;

    class UIManager final : public QObject {
        Q_OBJECT

    public:
        UIManager();
        ~UIManager() override = default;

    private:
        QVector<UI::ProjectManager*> projectManagers_;
        UI::FileSystem fs_;
        UI::SettingsSystem settingsSystem_;
        QTranslator* translator_{nullptr};
        UI::SettingsSystem::Language currentLanguage_{UI::SettingsSystem::Language::English};
        QVector<UI::ProjectData> collectProjectData() ;

        UI::ProjectManager* createStartWindow();
        UI::ProjectManager* createEditorProjectManager(QWindow* windowRender,
                                                       QLineEdit* consoleEngine);
        void deleteSlot(UI::ProjectManager* manager);
        void refreshFiles(UI::ProjectManager *manager) ;
        void initSignals(UI::ProjectManager& manager);

        void openFileInCurrentWindow(UI::ProjectManager* manager, qint64 fileId) ;
        void openFileByPathInCurrentWindow(UI::ProjectManager* manager, const QString& path) ;
        void openFileByPathInNewWindow(const QString& path);
        void createFileInCurrentWindow(UI::ProjectManager* manager, const QString& fileName) ;
        void createFileInNewWindow(const QString& fileName);
        void renameFile(UI::FileData& data) ;
        void deleteFile(UI::ProjectManager* manager, qint64 fileId) ;
        void openLightEditor(UI::ProjectManager *manager, UI::FileData& data);
        void returnTabFromLight(UI::ProjectManager* manager, qint64 fileId) ;
        void setActiveTab(UI::ProjectManager* manager, qint64 fileId) const;
        void goToStartWindow(UI::ProjectManager* manager) ;

        void sentCommandFromConsole(UI::ProjectManager* manager, qint64 fileId,
                                    const QString& command) const;
        void primitiveSlot(UI::ProjectManager* manager, UI::PrimitiveType& type, qint64 fileId);
        void constraintSlot(UI::ProjectManager* manager, UI::ConstraintType& type, qint64 fileId);
        void toolsSlot(UI::ProjectManager* manager, UI::ToolsType& type, qint64 fileId);
        void scriptSlot(UI::ProjectManager* manager, const QString& path);
        void settingsChangedSlot(UI::ProjectManager *manager);
        void applyLanguage(UI::SettingsSystem::Language lang);
        void closeTab(qint64 fileId);
    };

#endif //OURPAINT_DESKTOP_UI_UIMANAGER_H