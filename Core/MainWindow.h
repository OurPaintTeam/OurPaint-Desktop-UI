#ifndef OURPAINT_DESKTOP_UI_MAINWINDOW_H
#define OURPAINT_DESKTOP_UI_MAINWINDOW_H

#include "FramelessWindow.h"

#include <QVector>

class QWidget;
class QVBoxLayout;
class QStackedWidget;
class QCloseEvent;
class QString;


namespace UI {
    class StartPage;
    class ProjectPage;
    class TabPage;


    // Main application window
    class MainWindow final : public FramelessWindow {
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
        QWidget* contentWidget_{nullptr};
        QVBoxLayout* rootLayout_{nullptr};
        QStackedWidget* pageStack_{nullptr};

        StartPage* startPage_{nullptr};
        ProjectPage* projectPage_{nullptr};

        QVector<TabPage*> tabs_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_MAINWINDOW_H
