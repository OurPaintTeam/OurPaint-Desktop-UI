#ifndef OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
#define OURPAINT_DESKTOP_UI_PROJECTMANAGER_H

#include <qevent.h>
#include <QObject>
#include <QString>
#include <QVector>
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

        // Creates only StartWindow. ProjectWindow is created later via openProjectWindow().
        explicit ProjectManager();
        // Creates and opens ProjectWindow immediately (no StartWindow).
        ProjectManager(QWindow *windowRender, QLineEdit *consoleEngine);
        ~ProjectManager() override;

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

        void setFilesList(const QVector<FileData>& files);
        void addNotification(const qint64 id, const QString& text) const;
        void addNotification(const QString& text) const;

        // StartWindow is created (if needed) and ProjectWindow is removed.
        void openStartWindow();
        // ProjectWindow is created (or recreated), StartWindow is removed.
        void openNewWindow();
        void openNewWindow(QWindow *windowRender, QLineEdit *consoleEngine);
        void openLightEditorWindow(const QString& fileName,const qint64 id, QWindow *windowRender, QLineEdit *consoleEngine);
        void checkOpenedNewWindow(const qint64 id);

        void addTabSlot(const QString& name,const qint64 id);
        void deleteTabSlot(const qint64 id);
        void renameTabSlot(const qint64 id, const QString& newName);

        void setQOpenGLPainter(QOpenGLWindow *engine) const;
        void setQWindowRender(QWindow *engine) const;
        void setCommandConsoleEngine(QLineEdit *engine) const;

    signals:
        void sentCommandTriggered(const qint64 id, const QString command);
        void primitiveTriggered(const qint64 id, PrimitiveType type);
        void constraintTriggered(const qint64 id, ConstraintType type);
        void toolsTriggered(const qint64 id, ToolsType type);

        void saveFullProjectTriggered();
        void openFileThisWindowTriggered(const QString& path);
        void createFileThisWindowTriggered(const QString& path);
        void openNewWindowOpenFileTriggered(const QString& path);
        void openNewWindowCreateFileTriggered(const QString& path);
        void openLightEditorTriggered(const qint64 id);
        void renameFileTriggered(const qint64 id, const QString& path);
        void deleteFileTriggered(const qint64 id);
        void openFileTriggered(const QString& filePath);
        void createFileTriggered(const QString& path);
        void renameTabTriggered(const qint64 id, const QString& newName);
        void removeTabTriggered(const qint64 id);
        void closeTabTriggered(const qint64 id);
        void setActiveTabTriggered(const qint64 id);
        void returnLightEditorWindowTriggered(const qint64 id);
        void closeLightEditorWindowTriggered(const qint64 id);
        void scriptTriggered(const QString& text);
        void goToStartWindowTriggered();

        void applySettings(double value1, double value2, double value3);

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
        QVector<FileData> filesList_;

        StartWindow *startWindow_{nullptr};
        EditorWindow *editorWindow_{nullptr};
        QVector<LightEditorWindow*> lightEditorWindows_{};
        bool removingEditorWindow_{false};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTMANAGER_H
