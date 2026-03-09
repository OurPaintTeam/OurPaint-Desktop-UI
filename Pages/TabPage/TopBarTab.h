#ifndef OURPAINT_DESKTOP_UI_TOPBARTAB_H
#define OURPAINT_DESKTOP_UI_TOPBARTAB_H

#include "TopBar.h"

class SideTool;
class QLabel;
class QPushButton;

// Top bar for detached Tab
class TopBarTab final : public TopBar
{
    Q_OBJECT

public:
    explicit TopBarTab(QWidget* parent = nullptr);
    ~TopBarTab() override = default;

    void setLeftTool(SideTool* tool);
    void setRightTool(SideTool* tool);

private:
    // UI elements
    QLabel* title       {nullptr};
    SideTool* leftTool  {nullptr};
    SideTool* rightTool {nullptr};

    QPushButton* btnLeft  {nullptr};
    QPushButton* btnRight {nullptr};

    void createTabWidgets();
};

#endif // OURPAINT_DESKTOP_UI_TOPBARTAB_H