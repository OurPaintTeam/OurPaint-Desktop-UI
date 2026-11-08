#ifndef OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
#define OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H

#include <QString>

#include "BaseWindow.h"
#include "SettingsSystem.h"

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

        void setActiveID(const qint64& id);

        void setQOpenGLPainter(QOpenGLWindow *engine) const;
        void setQWindowRender(QWindow *engine) const;
        void setCommandConsoleEngine(QLineEdit *engine) const;

        virtual void setSettings(UI::SettingsSystem* settings) const;

    signals:
        void sentCommandTriggered(const qint64 id, const QString command);
        void primitiveTriggered(const qint64 id, PrimitiveType type);
        void constraintTriggered(const qint64 id, ConstraintType type);
        void toolsTriggered(const qint64 id, ToolsType type);

    protected:
        void initEditor(BaseEditorPage *editorPage);

        BaseEditorPage *editorPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORWINDOW_H
