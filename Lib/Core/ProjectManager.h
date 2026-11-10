#ifndef OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
#define OURPAINT_DESKTOP_UI_PROJECTMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>

#include "BaseWindow.h"
#include "FileSystem.h"
#include "ToolsType.h"
#include "Utils.h"

class QLineEdit;
class QWindow;
class QOpenGLWindow;
class QCloseEvent;

namespace UI {
    class SettingsSystem;
    class BaseEditorWindow;
    class StartWindow;
    class EditorWindow;
    class LightEditorWindow;

    struct EditorContext {
        FileSystem* fileSystem{nullptr};
        SettingsSystem* settingsSystem{nullptr};
        QWindow* renderEngine{nullptr};
        QLineEdit* consoleEngine{nullptr};
        SettingsSystem* settings{nullptr};
        QString defaultProjectPath;
        QString defaultScriptPath;
    };

    class ProjectManager final : public QObject {
        Q_OBJECT

    public:
        explicit ProjectManager(const EditorContext& context, QObject* parent = nullptr);
        ~ProjectManager() override;

        void addTabInEditor(const FileData& data);
        void openLightEditor(const EditorContext& context, const FileData& data);

        void addNotification(const qint64 id, const QString& text) const;
        void addNotification(const QString& text) const;
        void deleteTabSlot(const qint64 id);
        void renameTabSlot(FileData& data);
        void setActiveTab(const qint64 id);

        //  EditorWindow
        void setActiveToolEditorWindow(ToolsType tool) const;
        void setActiveToolEditorWindow(PrimitiveType tool) const;
        void setActiveToolEditorWindow(ConstraintType tool);
        void setHintConstraintToolsEditorWindow(const QVector<ConstraintType>& vecTools);
        void takeOffHintConstraintToolsEditorWindow();

        // LightEditorWindow
        void setActiveToolLightEditorWindow(qint64 id, ConstraintType tool);
        void takeOffHintConstraintToolsLightEditorWindow(qint64 id);
        void setHintConstraintToolsLightEditorWindow(qint64 id, const QVector<ConstraintType>& vecTools);
        void setActiveToolLightEditorWindow(qint64 id, ToolsType tool);
        void setActiveToolLightEditorWindow(qint64 id, PrimitiveType tool);

    signals:
        void createFileTriggered(const QString& path, bool newWindow);
        void openFileTriggered(const QString& path, bool newWindow);
        void openFileByIDTriggered(qint64 id, bool newWindow);
        void renameFileTriggered(FileData& data);
        void removeFileTriggered(const qint64 id);
        void closeFileTriggered(const qint64 id);
        void setActiveFileTriggered(const qint64 id);

        void sentCommandTriggered(const qint64 id, const QString command);
        void primitiveTriggered(const qint64 id, PrimitiveType type);
        void constraintTriggered(const qint64 id, ConstraintType type);
        void toolsTriggered(const qint64 id, ToolsType type);
        void scriptTriggered(const QString& text);

        void settingsChanged();
        void saveFullProjectTriggered();

        void lightEditorReturned(const qint64 id);
        void lightEditorRequested(const qint64 id);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        void showStartWindow(const QVector<ProjectData>& data);
        void returnToStartWindow();
        void handleEditorWindowClosed();

        void ensureStartWindow();
        void ensureEditorWindow();
        void connectStartWindowSignals();
        void connectEditorWindowSignals();
        void closeLightEditorWindows();

        LightEditorWindow* findLightEditorWindow(const qint64 id) const;
        BaseWindow* activeWindow() const;

        EditorContext context_;

        StartWindow *startWindow_{nullptr};
        EditorWindow *editorWindow_{nullptr};
        QVector<LightEditorWindow*> lightEditorWindows_{};
        QByteArray lastWindowGeometry_;
        bool removingEditorWindow_{false};
    };
} // namespace UI
#endif