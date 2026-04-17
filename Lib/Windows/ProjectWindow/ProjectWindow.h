#ifndef OURPAINT_DESKTOP_UI_PROJECTWINDOW_H
#define OURPAINT_DESKTOP_UI_PROJECTWINDOW_H

#include <QString>

#include "../BaseEditorWindow.h"

class QCloseEvent;

namespace UI {
    class ProjectPage;


    class ProjectWindow : public BaseEditorWindow {
        Q_OBJECT

    public:
        explicit ProjectWindow(QWidget* parent = nullptr);
        ~ProjectWindow() override = default;

        void initUI() override;
        void setDefaultProjectsPath(const QString& projectPath);
        void addTabSlot(const QString& name) const;
        void deleteTabSlot(const QString& name) const;
        void renameTabSlot(const QString& oldName, const QString& newName) const;

    signals:
        void windowClosedTriggered();
        void goToStartWindowTriggered();
        void openProjectThisWindowTriggered(const QString& path);
        void createProjectThisWindowTriggered(const QString& path);
        void openNewWindowOpenProjectTriggered(const QString& path);
        void openNewWindowCreateProjectTriggered(const QString& path);
        void openFileTriggered(const QString& filePath);
        void createFileTriggered(const QString& fileName);
        void renameTabTriggered(const QString& oldName, const QString& newName);
        void removeTabTriggered(const QString& fileName);
        void openTabWindowTriggered(const QString& tabName);
        void closeApplicationTriggered();

      void applySettings(double value1, double value2, double value3);
    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        ProjectPage* projectPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_PROJECTWINDOW_H
