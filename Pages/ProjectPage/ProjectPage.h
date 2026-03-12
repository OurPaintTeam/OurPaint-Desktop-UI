#ifndef OURPAINT_DESKTOP_UI_PROJECTPAGE_H
#define OURPAINT_DESKTOP_UI_PROJECTPAGE_H

#include <QWidget>

class QPushButton;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QString;

namespace UI {
    class TopBar;
    class TopBarProject;
    class TabBar;
    class ToolBar;
    class PainterWidget;
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
        QVBoxLayout* mainLayout_           {nullptr};
        QHBoxLayout* rootContentLayout_    {nullptr};
        QVBoxLayout* centerLayout_         {nullptr};
        QVBoxLayout* painterLayout_        {nullptr};
        QHBoxLayout* commandConsoleLayout_ {nullptr};

        // Widgets
        QWidget* mainArea_              {nullptr};
        QWidget* centerStack_           {nullptr};
        QWidget* painterWrapper_        {nullptr};
        QWidget* commandConsoleWrapper_ {nullptr};

        // UI elements
        TopBarProject* topBar_     {nullptr};
        TabBar* tabBar_            {nullptr};
        ToolBar* topToolBar_       {nullptr};
        PainterWidget* painter_        {nullptr};
        CommandConsole* commandConsole_ {nullptr};
        SideMenu* leftColumn_           {nullptr};
        SideMenu* rightColumn_          {nullptr};

        ContainerWidget* messengerContainer_ {nullptr};
        QPushButton* messengerButton_        {nullptr};
        ContainerWidget* infoContainer_      {nullptr};
        QPushButton* infoButton_             {nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_PROJECTPAGE_H