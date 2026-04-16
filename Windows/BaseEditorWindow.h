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

        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setQWindowRender(QWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;

    signals:
        void sentCommandTriggered(const QString& command);
        void primitiveTriggered(PrimitiveType& type);
        void constraintTriggered(ConstraintType& type);
        void toolsTriggered(ToolsType& type);

    protected:
        void initEditor(BaseEditorPage* editorPage);

        BaseEditorPage* editorPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
