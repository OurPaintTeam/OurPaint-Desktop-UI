#ifndef OURPAINT_DESKTOP_UI_TOPBARTAB_H
#define OURPAINT_DESKTOP_UI_TOPBARTAB_H

#include <QLabel>

#include "../BaseEditorTopBar.h"

namespace UI {
    class SideTool;


    // Top bar for detached Tab
    class TopBarTab final : public BaseEditorTopBar {
        Q_OBJECT

    public:
        explicit TopBarTab(QWidget* parent = nullptr);
        ~TopBarTab() override = default;

        void setLeftTool(SideTool* tool);
        void setRightTool(SideTool* tool);
        void setConsole(QWidget* console);
        signals:
        void returnTabWindowTriggered();

    private:
        // UI elements
        QLabel* title_{nullptr};

        void createTabWidgets();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARTAB_H
