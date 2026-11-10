#ifndef OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
#define OURPAINT_DESKTOP_UI_PROJECTMANAGER_H

#include <qevent.h>
#include <QObject>
#include <QString>
#include <QVector>

#include "SettingsSystem.h"
#include "Utils.h"


class QLineEdit;
class QWindow;
class QOpenGLWindow;

namespace UI {
    struct FileData;
    enum class ToolsType;
    enum class ConstraintType;
    enum class PrimitiveType;
    class BaseWindow;
    class BaseEditorWindow;
    class StartWindow;
    class EditorWindow;
    class LightEditorWindow;


    class ProjectManager final : public QObject {
        Q_OBJECT

    public:

        ProjectManager();
        void createStartWindow(const QVector<ProjectData>& data) ;
        void createEditorWindow(QWindow *windowRender, QLineEdit *consoleEngine, SettingsSystem* settingsSystem) ;
        void refreshFilesRequested(const QVector<ProjectData>& files);
        void setActiveToolEditorWindow(ToolsType tool) const;
        void setActiveToolEditorWindow(PrimitiveType tool) const;
        void setActiveToolEditorWindow(ConstraintType tool);
        void setHintConstraintToolsEditorWindow(const QVector<ConstraintType>& vecTools);
        void takeOffHintConstraintToolsEditorWindow();
        void setActiveToolLightEditorWindow(const qint64 id, ConstraintType tool);
        void takeOffHintConstraintToolsLightEditorWindow(const qint64 id);
        void setHintConstraintToolsLightEditorWindow(const qint64 id, const QVector<ConstraintType>& vecTools);
        void setActiveToolLightEditorWindow(const qint64 id, ToolsType tool);
        void setActiveToolLightEditorWindow(const qint64 id, PrimitiveType tool);

        void setActiveTab(const qint64 id);

        void setFilesList(const QVector<ProjectData>& files);
        void addNotification(const qint64 id, const QString& text) const;
        void addNotification(const QString& text) const;

        // StartWindow is created (if needed) and ProjectWindow is removed.
        void openStartWindow(const QVector<ProjectData>& data);
        // ProjectWindow is created (or recreated), StartWindow is removed.
        void reOpenNewEditorWindow();
        //void openNewWindow(QWindow *windowRender = nullptr, QLineEdit *consoleEngine = nullptr,  SettingsSystem* settings = nullptr);
        void openLightEditorWindow(FileData& data, QWindow *windowRender = nullptr, QLineEdit *consoleEngine = nullptr);
        void checkOpenedNewWindow(const qint64 id);
        bool editor();

        void addTabSlot(const FileData& data);
        ~ProjectManager();
        void deleteTabSlot(const qint64 id);
        void renameTabSlot(FileData& data);

        void setQOpenGLPainter(QOpenGLWindow *engine) const;
        void setQWindowRender(QWindow *engine) const;
        void setCommandConsoleEngine(QLineEdit *engine) const;
        void setSettings(SettingsSystem *settings) const;

    signals:
        void sentCommandTriggered(const qint64 id, const QString command);
        void primitiveTriggered(const qint64 id, PrimitiveType type);
        void constraintTriggered(const qint64 id, ConstraintType type);
        void toolsTriggered(const qint64 id, ToolsType type);

        void createFileTriggered(const QString& path,bool newWindow);
        void openFileTriggered(const QString& path,bool newWindow);
        void openFileByIDTriggered(qint64 id,bool newWindow);

        void renameFileTriggered(FileData& data);
        void removeFileTriggered(const qint64 id);
        void closeFileTriggered(const qint64 id);
        void setActiveFileTriggered(const qint64 id);

        void openLightEditorTriggered(const qint64 id);
        void returnLightEditorWindowTriggered(const qint64 id);
        void closeLightEditorWindowTriggered(const qint64 id);

        void saveFullProjectTriggered();
        void scriptTriggered(const QString& text);
        void goToStartWindowTriggered();

        void settingsChanged();

    protected:
        void closeEvent(QCloseEvent* event);

    private:
        void connectStartWindowSignals();
        void connectEditorWindowSignals();
        void ensureStartWindow();
        void ensureEditorWindow();
        void closeLightEditorWindows();
        void handleEditorWindowClosed();

        void removeEditorWindow();
        void removeStartWindow();

        LightEditorWindow* findTabWindow(const qint64 id) const;
        BaseWindow* activeWindow() const;
        BaseEditorWindow* activeEditorWindow() const;

        QString defaultFilePath_{};

        StartWindow *startWindow_{nullptr};
        EditorWindow *editorWindow_{nullptr};
        QVector<LightEditorWindow*> lightEditorWindows_{};
        bool removingEditorWindow_{false};

        SettingsSystem* settings_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
