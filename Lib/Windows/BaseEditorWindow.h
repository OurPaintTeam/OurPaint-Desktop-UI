#ifndef OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
#define OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H

#include <QString>

#include "BaseWindow.h"

class QLineEdit;
class QOpenGLWindow;
class QWindow;

namespace UI {
    enum class ConstraintType;
    enum class PrimitiveType;
    enum class ToolsType;
    class BaseEditorPage;


    class BaseEditorWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit BaseEditorWindow(QWidget *parent = nullptr);
        ~BaseEditorWindow() override = default;

        void setActiveTool(ToolsType tool);
        void setActiveTool(PrimitiveType tool);
        void setActiveTool(ConstraintType tool);
        void setHintConstraintTools(const QVector<ConstraintType>& vecTools);
        void takeOffHint();

        void setActiveName(const QString& name);
        void setSolverBackend(const QString& tabName, const QString& name, bool canSwitch);

        void setQOpenGLPainter(QOpenGLWindow *engine) const;
        void setQWindowRender(QWindow *engine) const;
        void setCommandConsoleEngine(QLineEdit *engine) const;

    signals:
        void sentCommandTriggered(const QString tabName, const QString command);
        void primitiveTriggered(const QString tabName, PrimitiveType type);
        void constraintTriggered(const QString tabName, ConstraintType type);
        void toolsTriggered(const QString tabName, ToolsType type);
        void solverBackendSwitchRequested(const QString& tabName);

    protected:
        void initEditor(BaseEditorPage *editorPage);

        BaseEditorPage *editorPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
