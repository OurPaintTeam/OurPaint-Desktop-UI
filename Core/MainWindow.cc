#include "MainWindow.h"

#include "NotificationContainer.h"
#include "ProjectPage.h"
#include "StartPage.h"
#include "TabPage.h"

#include <QCloseEvent>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {
    constexpr int MARGIN = 0;
    constexpr int SPACING = 0;
} // namespace


UI::MainWindow::MainWindow(QWidget* parent, UI::MainWindow::ProjectData data)
    : FramelessWindow(parent), projectData_(std::move((data))) {
    initUI();
}


UI::MainWindow::~MainWindow() {
    for (auto* tab: tabs_) {
        if (tab) {
            tab->close();
        }
    }
    destroyProjectPage();
}


QString UI::MainWindow::projectPath() const {
    return projectData_.path;
}


QString UI::MainWindow::projectID() const {
    return projectData_.id;
}


void UI::MainWindow::setProjectPath(const QString& projectPath) {
    projectData_.path = projectPath;
}


void UI::MainWindow::setDefaultProjectsPath(const QString& projectPath) {
    startPage_->setDefaultProjectsPath(projectPath);
    defaultProjectPath_ = projectPath;
}


void UI::MainWindow::setProjectsList(const QVector<QPair<QString, QString> >& projectsList) const {
    startPage_->setProjectsList(projectsList);
}


void UI::MainWindow::addNotification(const QString& text) const {
    notificationManager_->addNotification(text);
}


void UI::MainWindow::setQOpenGLPainter(QOpenGLWindow* engine) const {
    projectPage_->setQOpenGLPainter(engine);
}


void UI::MainWindow::setCommandConsoleEngine(QLineEdit* engine) const {
    projectPage_->setCommandConsoleEngine(engine);
}


void UI::MainWindow::onStartWindowSlot() {
    projectData_ = {};
    for (auto* tab: tabs_) {
        if (tab) {
            tab->close();
        }
    }
    tabs_.clear();

    destroyProjectPage();
    pageStack_->setCurrentWidget(startPage_);
}


void UI::MainWindow::onOpenProjectSlot(const UI::MainWindow::ProjectData& data) {
    projectData_ = data;
    for (auto* tab: tabs_) {
        if (tab) {
            tab->close();
        }
    }
    tabs_.clear();

    destroyProjectPage();
    createProjectPage();
    pageStack_->setCurrentWidget(projectPage_);
}


void UI::MainWindow::addTabSlot(const QString& name) const {
    if (projectPage_) {
        projectPage_->onAddTabSlot(name);
    }
}


void UI::MainWindow::deleteTabSlot(const QString& name) const {
    if (projectPage_) {
        projectPage_->onDeleteTabSlot(name);
    }
}


void UI::MainWindow::onRenameTabSlot(const QString& oldName, const QString& newName) const {
    if (projectPage_) {
        projectPage_->onRenameTabSlot(oldName, newName);
    }
}


void UI::MainWindow::openTabPageSlot(const QString& tabName) {
    auto* tab = new TabPage(nullptr);
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->setWindowTitle(tabName);
    tabs_.append(tab);

    connect(tab, &QObject::destroyed, this, [this, tab]() { tabs_.removeAll(tab); });
    tab->show();
}


void UI::MainWindow::initUI() {
    contentWidget_ = new QWidget(this);
    setCentralWidget(contentWidget_);

    rootLayout_ = new QVBoxLayout(contentWidget_);
    rootLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    rootLayout_->setSpacing(SPACING);

    pageStack_ = new QStackedWidget(contentWidget_);
    rootLayout_->addWidget(pageStack_);

    // always exists
    startPage_ = new StartPage(pageStack_);
    pageStack_->addWidget(startPage_);
    pageStack_->setCurrentWidget(startPage_);

    notificationManager_ = new NotificationContainer(this);

    // signals
    connect(startPage_, &StartPage::openProjectInThisWindowTriggered,
            this, &UI::MainWindow::openProjectThisWindowTriggered);

    connect(startPage_, &StartPage::createProjectInThisWindowTriggered,
            this, &UI::MainWindow::createProjectThisWindowTriggered);

    connect(startPage_, &StartPage::renameProjectTriggered,
            this, &UI::MainWindow::renameProjectTriggered);

    connect(startPage_, &StartPage::deleteProjectTriggered,
            this, &UI::MainWindow::deleteProjectTriggered);
}


void UI::MainWindow::closeEvent(QCloseEvent* event) {
    for (auto* tab: tabs_) {
        if (tab) {
            tab->close();
        }
    }
    tabs_.clear();
    FramelessWindow::closeEvent(event);
}


void UI::MainWindow::createProjectPage() {
    if (projectPage_) {
        return;
    }

    projectPage_ = new ProjectPage(pageStack_);
    pageStack_->addWidget(projectPage_);
    projectPage_->setDefaultProjectsPath(defaultProjectPath_);


    connect(projectPage_, &ProjectPage::goToStartWindowTriggered,
            this, &UI::MainWindow::goToStartWindowTriggered);
    connect(projectPage_, &ProjectPage::openTabWindowTriggered,
            this, &UI::MainWindow::openTabPageSlot);
    connect(projectPage_, &ProjectPage::createFileTriggered,
            this, &UI::MainWindow::createFileTriggered);
    connect(projectPage_, &ProjectPage::openFileTriggered,
            this, &UI::MainWindow::openFileTriggered);
    connect(projectPage_, &ProjectPage::renameTabTriggered,
            this, &UI::MainWindow::renameTabTriggered);
    connect(projectPage_, &ProjectPage::removeTabTriggered,
            this, &UI::MainWindow::removeTabTriggered);

    connect(projectPage_, &ProjectPage::openProjectThisWindowTriggered,
            this, &UI::MainWindow::openProjectThisWindowTriggered);
    connect(projectPage_, &ProjectPage::createProjectThisWindowTriggered,
            this, &UI::MainWindow::createProjectThisWindowTriggered);
    connect(projectPage_, &ProjectPage::openNewWindowOpenProjectTriggered,
            this, &UI::MainWindow::openNewWindowOpenProjectTriggered);
    connect(projectPage_, &ProjectPage::openNewWindowCreateProjectTriggered,
            this, &UI::MainWindow::openNewWindowCreateProjectTriggered);
}


void UI::MainWindow::destroyProjectPage() {
    if (!projectPage_) {
        return;
    }

    pageStack_->removeWidget(projectPage_);
    projectPage_->deleteLater();
    projectPage_ = nullptr;
}
