#ifndef OURPAINT_DESKTOP_UI_MAINWINDOW_H
#define OURPAINT_DESKTOP_UI_MAINWINDOW_H

#include "FramelessWindow.h"

#include <QVector>

class QWidget;
class QVBoxLayout;
class QStackedWidget;
class QCloseEvent;
class QString;

class StartPage;
class ProjectPage;
class TabPage;

// Main application window
class MainWindow final : public FramelessWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public slots:
    void openTabPage(const QString& tabName);

protected:
    void initUI() override;
    void closeEvent(QCloseEvent* event) override;

private:
    QWidget* m_contentWidget    = nullptr;
    QVBoxLayout* m_rootLayout   = nullptr;
    QStackedWidget* m_pageStack = nullptr;

    StartPage* m_startPage      = nullptr;
    ProjectPage* m_projectPage  = nullptr;

    QVector<TabPage*> m_tabs;
};

#endif // OURPAINT_DESKTOP_UI_MAINWINDOW_H