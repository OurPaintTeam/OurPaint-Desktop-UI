#ifndef OURPAINT_DESKTOP_UI_BASEEDITORPAGE_H
#define OURPAINT_DESKTOP_UI_BASEEDITORPAGE_H

#include <QString>

#include "BasePage.h"

class QHBoxLayout;
class QLineEdit;
class QOpenGLWindow;
class QVBoxLayout;
class QWindow;

namespace UI {
    enum class ConstraintType;
    enum class PrimitiveType;
    enum class ToolsType;
    class CommandConsole;
    class PainterWidget;
    class ToolBar;


    class BaseEditorPage : public BasePage {
        Q_OBJECT

    public:
        explicit BaseEditorPage(QWidget* parent = nullptr,const QString& name = "");
        QString name() const;
        ~BaseEditorPage() override = default;

        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setQWindowRender(QWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;
        void setActiveTool(ToolsType tool);
        void setActiveTool(PrimitiveType tool);
        void setActiveName(const QString& name);

    signals:
        void sentCommandTriggered(const QString tabName, const QString& command);
        void primitiveTriggered(const QString tabName,PrimitiveType& type);
        void constraintTriggered(const QString tabName,ConstraintType& type);
        void toolsTriggered(const QString tabName, ToolsType& type);

    protected:
        void initBaseEditorPage();
        QWidget* createWorkspacePage(QWidget* parent);
        virtual void initUI() = 0;

        QHBoxLayout* rootLayout_{nullptr};
        QVBoxLayout* centerLayout_{nullptr};
        QVBoxLayout* workspaceLayout_{nullptr};
        QVBoxLayout* painterLayout_{nullptr};
        QHBoxLayout* consoleLayout_{nullptr};

        QWidget* centerWidget_{nullptr};
        QWidget* painterWrapper_{nullptr};
        QWidget* consoleWrapper_{nullptr};

        ToolBar* toolBar_{nullptr};
        PainterWidget* painter_{nullptr};
        CommandConsole* console_{nullptr};
        QString activeName_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORPAGE_H
