#ifndef OURPAINT_DESKTOP_UI_TABPAGE_H
#define OURPAINT_DESKTOP_UI_TABPAGE_H

#include "FramelessWindow.h"

class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QString;

class TopBarTab;
class ToolBar;
class QTPainter;
class CommandConsole;
class SideTool;

// Detached CAD workspace tab
class TabPage final : public FramelessWindow
{
    Q_OBJECT

public:
    explicit TabPage(QWidget* parent = nullptr);
    ~TabPage() override;

    signals:
        void returnTab(const QString& tabName);

protected:
    void initUI() override;

private:
    // Layouts
    QVBoxLayout* m_mainLayout    {nullptr};
    QHBoxLayout* m_rootLayout    {nullptr};
    QVBoxLayout* m_centerLayout  {nullptr};
    QVBoxLayout* m_painterLayout {nullptr};
    QHBoxLayout* m_consoleLayout {nullptr};

    // Wrapper widgets
    QWidget* centralWidget      {nullptr};
    QWidget* m_mainArea         {nullptr};
    QWidget* m_centerStack      {nullptr};
    QWidget* m_painterWrapper   {nullptr};
    QWidget* m_consoleWrapper   {nullptr};

    // UI elements
    TopBarTab* m_topBar   {nullptr};
    SideTool* m_leftTool  {nullptr};
    SideTool* m_rightTool {nullptr};

    ToolBar* m_toolBar        {nullptr};
    QTPainter* m_painter      {nullptr};
    CommandConsole* m_console {nullptr};
};

#endif // OURPAINT_DESKTOP_UI_TABPAGE_H