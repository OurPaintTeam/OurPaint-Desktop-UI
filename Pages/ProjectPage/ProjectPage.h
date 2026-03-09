#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QWidget>

class QPushButton;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QString;

class TopBar;
class TopBarProject;
class TabBar;
class ToolBar;
class QTPainter;
class CommandConsole;
class SideMenu;
class ContainerWidget;

// Main in-window CAD workspace
class ProjectPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectPage(QWidget* parent = nullptr);
    ~ProjectPage() override = default;

    signals:
        void requestOpenTabPage(const QString& fileName);
    void requestCloseProject();

private:
    void initUI();

private:
    // Layouts
    QVBoxLayout* m_mainLayout           {nullptr};
    QHBoxLayout* m_rootContentLayout    {nullptr};
    QVBoxLayout* m_centerLayout         {nullptr};
    QVBoxLayout* m_painterLayout        {nullptr};
    QHBoxLayout* m_commandConsoleLayout {nullptr};

    // Widgets
    QWidget* m_mainArea              {nullptr};
    QWidget* m_centerStack           {nullptr};
    QWidget* m_painterWrapper        {nullptr};
    QWidget* m_commandConsoleWrapper {nullptr};

    // UI elements
    TopBarProject* m_topBar     {nullptr};
    TabBar* m_tabBar            {nullptr};
    ToolBar* m_topToolBar       {nullptr};
    QTPainter* m_painter        {nullptr};
    CommandConsole* m_commandConsole {nullptr};
    SideMenu* m_leftColumn           {nullptr};
    SideMenu* m_rightColumn          {nullptr};

    ContainerWidget* messengerContainer {nullptr};
    QPushButton* messengerButton        {nullptr};
    ContainerWidget* infoContainer      {nullptr};
    QPushButton* infoButton             {nullptr};
};

#endif // OURPAINT_DESKTOP_UI_PROJECTPAGE_H