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
class QCloseEvent;

namespace UI {
    class SettingsSystem;
    class BaseEditorWindow;
    class MainWindow;
    class LightEditorWindow;

    struct EditorContext {
        FileSystem* fileSystem{nullptr};
        SettingsSystem* settings{nullptr};
        QWindow* renderEngine{nullptr};
        QLineEdit* consoleEngine{nullptr};
    };

    class ProjectManager final : public QObject {
        Q_OBJECT

    public:
        explicit ProjectManager(const EditorContext& context, QObject* parent = nullptr);
        ~ProjectManager() override;

        void showStartWindow();
        void addTabSlot(FileData& data);
        void showEditorWindow(FileData& data);
        void showLightEditorWindow(const FileData& data);
        bool isLightEditor() const;

        void setActiveTab(const qint64 id);
        void closeTabSlot(const qint64 id);
        void deleteTabSlot(const qint64 id);
        void renameTabSlot(FileData& data);

        void addNotification(const qint64 id, const QString& text) const;
        void addNotification(const QString& text) const;

        void setActiveTool(ToolsType tool) const;
        void setActiveTool(PrimitiveType tool) const;
        void setActiveTool(ConstraintType tool) const;
        void setHintConstraintTools(const QVector<ConstraintType>& vecTools) const;
        void takeOffHintConstraintTools() const;

        BaseWindow* activeWindow() const;
        QWidget* windowWidget() const;
    signals:
        void createFileTriggered(const QString& path, bool newWindow);
        void openFileTriggered(const QString& path, bool newWindow);
        void openFileByIDTriggered(qint64 id, bool newWindow);
        void renameFileTriggered(FileData& data);
        void removeFileTriggered(const qint64 id);
        void closeFileTriggered(const qint64 id);
        void setActiveFileTriggered(const qint64 id);
        void tabMovedToAnotherWindow(qint64 fileId, quintptr sourceWindowPtr, quintptr targetWindowPtr);

        void sentCommandTriggered(const qint64 id, const QString command);
        void primitiveTriggered(const qint64 id, PrimitiveType type);
        void constraintTriggered(const qint64 id, ConstraintType type);
        void toolsTriggered(const qint64 id, ToolsType type);
        void scriptTriggered(const QString& text);

        void settingsChanged();
        void saveFullProjectTriggered();

        void lightEditorRequested(const qint64 id);
        void returnTabToEditorRequested(qint64 id);
        void closeLightEditorWindowTriggered(const qint64 id);

    private:
        void ensureMainWindow();
        void ensureLightEditorWindow(const FileData& data);
        void loadSettings() const;
        void connectMainWindowSignals();

        void showStartWindow(const QVector<ProjectData>& data);
        void returnToStartWindow();
        void handleEditorWindowClosed();

        EditorContext context_;

        MainWindow* mainWindow_{nullptr};
        LightEditorWindow* lightEditorWindow_{nullptr};
    };
}

#endif