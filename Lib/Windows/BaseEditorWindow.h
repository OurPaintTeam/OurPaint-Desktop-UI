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
        explicit BaseEditorWindow(QWidget* parent = nullptr);
        ~BaseEditorWindow() override = default;

      void setActiveTool(ToolsType tool);
      void setActiveTool(PrimitiveType tool);
        void setActiveName(const QString& name);

        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setQWindowRender(QWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;

    signals:
        void sentCommandTriggered(const QString tabName, const QString& command);
        void primitiveTriggered(const QString tabName,PrimitiveType& type);
        void constraintTriggered(const QString tabName,ConstraintType& type);
        void toolsTriggered(const QString tabName, ToolsType& type);

    protected:
        void initEditor(BaseEditorPage* editorPage);

        BaseEditorPage* editorPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
