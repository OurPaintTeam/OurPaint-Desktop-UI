#ifndef OURPAINT_DESKTOP_UI_TOPBARTAB_H
#define OURPAINT_DESKTOP_UI_TOPBARTAB_H

#include <QLabel>

#include "TopBar.h"

namespace UI {
    class CommandConsole;
    class SideTool;


    // Top bar for detached Tab
    class TopBarTab final : public TopBar {
        Q_OBJECT

    public:
        explicit TopBarTab(QWidget* parent = nullptr);
        ~TopBarTab() override = default;

        void setLeftTool(SideTool* tool);
        void setRightTool(SideTool* tool);
        void setCommandConsole(CommandConsole* console);

    private:
        // UI elements
        QLabel* title_{nullptr};
        SideTool* leftTool_{nullptr};
        SideTool* rightTool_{nullptr};
        CommandConsole* commandConsole_{nullptr};

        QPushButton* btnLeft_{nullptr};
        QPushButton* btnRight_{nullptr};
        QPushButton* btnConsole_{nullptr};

        void createTabWidgets();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARTAB_H
