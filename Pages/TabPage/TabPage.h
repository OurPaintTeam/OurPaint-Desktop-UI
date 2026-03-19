#ifndef OURPAINT_DESKTOP_UI_TABPAGE_H
#define OURPAINT_DESKTOP_UI_TABPAGE_H

#include "FramelessWindow.h"

class QVBoxLayout;
class QHBoxLayout;
class QString;
class QWidget;


namespace UI {
    class PainterWidget;
    class TopBarTab;
    class ToolBar;
    class CommandConsole;
    class SideTool;


    // Detached CAD workspace tab
    class TabPage final : public FramelessWindow {
        Q_OBJECT

    public:
        explicit TabPage(QWidget* parent = nullptr);
        ~TabPage() override;

    signals:
        void returnTab(const QString& tabName);
        void sentCommandTriggered(const QString& command);

    protected:
        void initUI() override;

    private:
        // Layouts
        QVBoxLayout* mainLayout_{nullptr};
        QHBoxLayout* rootLayout_{nullptr};
        QVBoxLayout* centerLayout_{nullptr};
        QVBoxLayout* painterLayout_{nullptr};
        QHBoxLayout* consoleLayout_{nullptr};

        // Wrapper widgets
        QWidget* centralWidget_{nullptr};
        QWidget* mainArea_{nullptr};
        QWidget* centerStack_{nullptr};
        QWidget* painterWrapper_{nullptr};
        QWidget* consoleWrapper_{nullptr};

        // UI elements
        TopBarTab* topBar_{nullptr};
        SideTool* leftTool_{nullptr};
        SideTool* rightTool_{nullptr};

        ToolBar* toolBar_{nullptr};
        PainterWidget* painter_{nullptr};
        CommandConsole* console_{nullptr};
    };
} // namespace UI
#endif // OURPAINT_DESKTOP_UI_TABPAGE_H
