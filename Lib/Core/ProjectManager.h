#ifndef OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
#define OURPAINT_DESKTOP_UI_PROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>

class QLineEdit;
class QWindow;
class QOpenGLWindow;

namespace UI {
    enum class ToolsType;
    enum class ConstraintType;
    enum class PrimitiveType;
    class BaseWindow;
    class BaseEditorWindow;
    class StartWindow;
    class ProjectWindow;
    class TabWindow;


    class ProjectManager final : public QObject {
        Q_OBJECT

    public:
        struct ProjectData {
            QString path{};
            QString id{};
        };


        // Creates only StartWindow. ProjectWindow is created later via openProjectWindow().
        explicit ProjectManager(ProjectData data = {{}, {}});
        // Creates and opens ProjectWindow immediately (no StartWindow).
        ProjectManager(ProjectData data, QWindow* windowRender, QLineEdit* consoleEngine);
        ~ProjectManager() override;

      void setActiveToolProjectWindow(ToolsType tool) const;
      void setActiveToolProjectWindow(PrimitiveType tool);
      void setActiveToolTabWindow(const QString& tabName, ToolsType tool);
      void setActiveToolTabWindow(const QString& tabName, PrimitiveType tool);

        QString projectPath() const;
        QString projectID() const;
        void setProjectPath(const QString& projectPath);
        void setProjectData(const ProjectData& data);

        void setDefaultProjectsPath(const QString& projectPath);
        void setProjectsList(const QVector<QPair<QString, QString> >& projectsList);
        void addNotification(const QString& text) const;

        // StartWindow is created (if needed) and ProjectWindow is removed.
        void openStartWindow();
        // ProjectWindow is created (or recreated), StartWindow is removed.
        void openProjectWindow();
        void openProjectWindow(const ProjectData& data, QWindow* windowRender, QLineEdit* consoleEngine);
        void openTabWindow(const QString& tabName, QWindow* windowRender, QLineEdit* consoleEngine);
        void checkOpenedTabWindow(const QString& tabName);

        void addTabSlot(const QString& name);
        void deleteTabSlot(const QString& name);
        void renameTabSlot(const QString& oldName, const QString& newName);

        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setQWindowRender(QWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;

        signals:
        void sentCommandTriggered(const QString& command);
        void primitiveTriggered(PrimitiveType& type);
        void constraintTriggered(ConstraintType& type);
        void toolsTriggered(ToolsType& type);

        void openProjectThisWindowTriggered(const QString& path);
        void createProjectThisWindowTriggered(const QString& path);
        void openNewWindowOpenProjectTriggered(const QString& path);
        void openNewWindowCreateProjectTriggered(const QString& path);
        void openNewWindowTabTriggered(const QString& name);
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);
        void openFileTriggered(const QString& filePath);
        void createFileTriggered(const QString& fileName);
        void renameTabTriggered(const QString& oldName, const QString& newName);
        void removeTabTriggered(const QString& fileName);
        void goToStartWindowTriggered();

      void applySettings(double value1, double value2, double value3);

    private:
        void connectStartWindowSignals();
        void connectProjectWindowSignals();
        void ensureStartWindow();
        void ensureProjectWindow();
        void closeTabWindows();
        void handleProjectWindowClosed();

        void removeProjectWindow();
        void removeStartWindow();

        TabWindow* findTabWindow(const QString& tabName) const;
        BaseWindow* activeWindow() const;
        BaseEditorWindow* activeEditorWindow() const;

        ProjectData projectData_{};
        QString defaultProjectPath_{};
        QVector<QPair<QString, QString> > projectsList_{};
        QVector<QString> tabs_{};

        StartWindow* startWindow_{nullptr};
        ProjectWindow* projectWindow_{nullptr};
        QVector<TabWindow*> tabWindows_{};
        bool removingProjectWindow_{false};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
