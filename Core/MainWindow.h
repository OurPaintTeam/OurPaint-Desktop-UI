#ifndef OURPAINT_DESKTOP_UI_MAINWINDOW_H
#define OURPAINT_DESKTOP_UI_MAINWINDOW_H

#include "FramelessWindow.h"

class QLineEdit;
class QOpenGLWindow;
class QWidget;
class QVBoxLayout;
class QStackedWidget;
class QCloseEvent;
class QString;

namespace UI {
    class NotificationContainer;
    class PainterWidget;
    class StartPage;
    class ProjectPage;
    class TabPage;


    class MainWindow final : public FramelessWindow {
        Q_OBJECT

    public:
        struct ProjectData {
            QString path;
            QString id;
        };


        explicit MainWindow(QWidget* parent = nullptr, MainWindow::ProjectData data = {});
        ~MainWindow() override;

        QString projectPath() const;
        QString projectID() const;
        void setProjectPath(const QString& projectPath);
        void setDefaultProjectsPath(const QString& projectPath);
        void setProjectsList(const QVector<QPair<QString, QString> >& projectsList) const;

        void addNotification(const QString& text) const;

        void setQOpenGLPainter(QOpenGLWindow* engine) const;
        void setQWindowRender(QWindow* engine) const;
        void setCommandConsoleEngine(QLineEdit* engine) const;

    signals:
        void goToStartWindowTriggered();

        void openProjectThisWindowTriggered(const QString& path);
        void createProjectThisWindowTriggered(const QString& path);
        void openNewWindowOpenProjectTriggered(const QString& path);
        void openNewWindowCreateProjectTriggered(const QString& path);
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);

        void openFileTriggered(const QString& filePath);
        void createFileTriggered(const QString& fileName);

        void renameTabTriggered(const QString& oldName, const QString& newName);
        void removeTabTriggered(const QString& fileName);

        void closeApplication(const QString& projectPath);

        /// Console
        void sentCommandTriggered(const QString& command);

        /// Tools - constrains
        void pointLineDistanceTriggered();
        void pointOnLineTriggered();
        void pointPointDistanceTriggered();
        void coincidentPointsTriggered();
        void lineCircleDistanceTriggered();
        void lineOnCircleTriggered();
        void lineInCircleTriggered();
        void parallelLinesTriggered();
        void perpendicularLinesTriggered();
        void angleBetweenLinesTriggered();

        /// Tools - point
        void pointTriggered();

        /// Tools - line
        void lineTriggered();
        void polylineTriggered();
        void infiniteLineTriggered();

        /// Tools - circle
        void circleByDiameterTriggered();
        void circleTwoPointsTriggered();
        void ellipseThreePointsTriggered();

        /// Tools - arc
        void arcByRadiusTriggered();
        void arcByDiameterTriggered();
        void arcByThreePointsTriggered();

        /// Tools - settings
        void lineSettingsTriggered();
        void circleSettingsTriggered();
        void arcSettingsTriggered();

        /// Tools - tool
        void cursorTriggered();
        void sizeTriggered();
    public slots:
        void onStartWindowSlot();

        void onOpenProjectSlot(const UI::MainWindow::ProjectData& data);

        void addTabSlot(const QString& name) const;
        void deleteTabSlot(const QString& name) const;
        void onRenameTabSlot(const QString& oldName, const QString& newName) const;
        void openTabPageSlot(const QString& tabName);

    protected:
        void initUI() override;
        void closeEvent(QCloseEvent* event) override;

    private:
        void createProjectPage();
        void destroyProjectPage();

        QWidget* contentWidget_{nullptr};
        QVBoxLayout* rootLayout_{nullptr};
        QStackedWidget* pageStack_{nullptr};

        StartPage* startPage_{nullptr};
        ProjectPage* projectPage_{nullptr};

        QVector<TabPage*> tabs_{};

        ProjectData projectData_{};

        NotificationContainer* notificationManager_{nullptr};

        QString defaultProjectPath_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_MAINWINDOW_H
