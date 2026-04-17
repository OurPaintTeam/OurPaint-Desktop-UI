#include "ProjectManager.h"

#include "BaseEditorWindow.h"
#include "BaseWindow.h"
#include "ProjectWindow.h"
#include "StartWindow.h"
#include "TabWindow.h"


UI::ProjectManager::ProjectManager(ProjectManager::ProjectData data)
    : projectData_(std::move(data)) {
    // For this constructor we always start with StartWindow.
    ensureStartWindow();
    openStartWindow();
}


UI::ProjectManager::ProjectManager(ProjectManager::ProjectData data, QWindow* windowRender,
                                   QLineEdit* consoleEngine)
    : projectData_(std::move(data)) {
    // No StartWindow. Create ProjectWindow and apply engines.
    openProjectWindow();
    if (windowRender) {
        setQWindowRender(windowRender);
    }
    if (consoleEngine) {
        setCommandConsoleEngine(consoleEngine);
    }
}


UI::ProjectManager::~ProjectManager() {
    closeTabWindows();

    if (projectWindow_) {
        projectWindow_->close();
        projectWindow_->deleteLater();
    }

    if (startWindow_) {
        startWindow_->close();
        startWindow_->deleteLater();
    }
}


QString UI::ProjectManager::projectPath() const {
    return projectData_.path;
}


QString UI::ProjectManager::projectID() const {
    return projectData_.id;
}


void UI::ProjectManager::setProjectPath(const QString& projectPath) {
    projectData_.path = projectPath;
}


void UI::ProjectManager::setProjectData(const ProjectData& data) {
    projectData_ = data;
}


void UI::ProjectManager::setDefaultProjectsPath(const QString& projectPath) {
    defaultProjectPath_ = projectPath;
    if (startWindow_) {
        startWindow_->setDefaultProjectsPath(projectPath);
    }
    if (projectWindow_) {
        projectWindow_->setDefaultProjectsPath(projectPath);
    }
}


void UI::ProjectManager::setProjectsList(const QVector<QPair<QString, QString> >& projectsList) {
    projectsList_ = projectsList;
    if (startWindow_) {
        startWindow_->setProjectsList(projectsList_);
    }
}


void UI::ProjectManager::addNotification(const QString& text) const {
    if (const auto* window = activeWindow()) {
        window->addNotification(text);
    }
}


void UI::ProjectManager::openStartWindow() {
    projectData_ = {};
    closeTabWindows();
    tabs_.clear();

    removeProjectWindow();
    ensureStartWindow();

    if (!projectsList_.isEmpty()) {
        startWindow_->setProjectsList(projectsList_);
    }
    if (!defaultProjectPath_.isEmpty()) {
        startWindow_->setDefaultProjectsPath(defaultProjectPath_);
    }

    startWindow_->show();
    startWindow_->raise();
    startWindow_->activateWindow();
}


void UI::ProjectManager::openProjectWindow() {
    removeStartWindow();
    closeTabWindows();
    tabs_.clear();

    removeProjectWindow();
    ensureProjectWindow();

    if (!defaultProjectPath_.isEmpty()) {
        projectWindow_->setDefaultProjectsPath(defaultProjectPath_);
    }

    projectWindow_->show();
    projectWindow_->raise();
    projectWindow_->activateWindow();
}


void UI::ProjectManager::openProjectWindow(const ProjectData& data, QWindow* windowRender, QLineEdit* consoleEngine) {
    openProjectWindow();
    if (windowRender) {
        setQWindowRender(windowRender);
    }
    if (consoleEngine) {
        setCommandConsoleEngine(consoleEngine);
    }
    projectData_ = data;
}


void UI::ProjectManager::addTabSlot(const QString& name) {
    if (name.isEmpty() || tabs_.contains(name)) {
        return;
    }

    tabs_.append(name);
    projectWindow_->addTabSlot(name);
}


void UI::ProjectManager::deleteTabSlot(const QString& name) {
    if (name.isEmpty()) {
        return;
    }

    tabs_.removeAll(name);
    projectWindow_->deleteTabSlot(name);

    if (auto* tabWindow = findTabWindow(name)) {
        tabWindows_.removeOne(tabWindow);
        tabWindow->close();
        tabWindow->deleteLater();
    }
}


void UI::ProjectManager::renameTabSlot(const QString& oldName, const QString& newName) {
    if (oldName.isEmpty() || newName.isEmpty()) {
        return;
    }

    const auto index = tabs_.indexOf(oldName);
    if (index >= 0) {
        tabs_[index] = newName;
    }

    projectWindow_->renameTabSlot(oldName, newName);

    if (auto* tabWindow = findTabWindow(oldName)) {
        tabWindow->setTabName(newName);
    }
}


void UI::ProjectManager::setQOpenGLPainter(QOpenGLWindow* engine) const {
    if (const auto* window = activeEditorWindow()) {
        window->setQOpenGLPainter(engine);
    }
}


void UI::ProjectManager::setQWindowRender(QWindow* engine) const {
    if (const auto* window = activeEditorWindow()) {
        window->setQWindowRender(engine);
    }
}


void UI::ProjectManager::setCommandConsoleEngine(QLineEdit* engine) const {
    if (const auto* window = activeEditorWindow()) {
        window->setCommandConsoleEngine(engine);
    }
}


void UI::ProjectManager::connectStartWindowSignals() {
    if (!startWindow_) {
        return;
    }

    connect(startWindow_, &StartWindow::openProjectThisWindowTriggered,
            this, &ProjectManager::openProjectThisWindowTriggered);

    connect(startWindow_, &StartWindow::createProjectThisWindowTriggered,
            this, &ProjectManager::createProjectThisWindowTriggered);

    connect(startWindow_, &StartWindow::renameProjectTriggered,
            this, &ProjectManager::renameProjectTriggered);

    connect(startWindow_, &StartWindow::deleteProjectTriggered,
            this, &ProjectManager::deleteProjectTriggered);
}


void UI::ProjectManager::connectProjectWindowSignals() {
    if (!projectWindow_) {
        return;
    }

    connect(projectWindow_, &ProjectWindow::windowClosedTriggered,
            this, &ProjectManager::handleProjectWindowClosed);

    connect(projectWindow_, &ProjectWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);

    connect(projectWindow_, &ProjectWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);

    connect(projectWindow_, &ProjectWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);

    connect(projectWindow_, &ProjectWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);

    connect(projectWindow_, &ProjectWindow::goToStartWindowTriggered,
            this, &ProjectManager::goToStartWindowTriggered);

    connect(projectWindow_, &ProjectWindow::openProjectThisWindowTriggered,
            this, &ProjectManager::openProjectThisWindowTriggered);

    connect(projectWindow_, &ProjectWindow::createProjectThisWindowTriggered,
            this, &ProjectManager::createProjectThisWindowTriggered);

    connect(projectWindow_, &ProjectWindow::openNewWindowOpenProjectTriggered,
            this, &ProjectManager::openNewWindowOpenProjectTriggered);

    connect(projectWindow_, &ProjectWindow::openNewWindowCreateProjectTriggered,
            this, &ProjectManager::openNewWindowCreateProjectTriggered);

    connect(projectWindow_, &ProjectWindow::openFileTriggered,
            this, &ProjectManager::openFileTriggered);

    connect(projectWindow_, &ProjectWindow::createFileTriggered,
            this, &ProjectManager::createFileTriggered);

    connect(projectWindow_, &ProjectWindow::renameTabTriggered,
            this, &ProjectManager::renameTabTriggered);

    connect(projectWindow_, &ProjectWindow::removeTabTriggered,
            this, &ProjectManager::removeTabTriggered);

    connect(projectWindow_, &ProjectWindow::openTabWindowTriggered,
            this, &ProjectManager::checkOpenedTabWindow);

    connect(projectWindow_, &ProjectWindow::closeApplicationTriggered,
            this, [this] {
                if (auto* window = activeWindow()) {
                    window->close();
                }
            });

  connect(projectWindow_, &ProjectWindow::applySettings,
        this, &ProjectManager::applySettings);
}


void UI::ProjectManager::ensureStartWindow() {
    if (startWindow_) {
        return;
    }

    startWindow_ = new StartWindow();
    startWindow_->hide();
    startWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectStartWindowSignals();
}


void UI::ProjectManager::ensureProjectWindow() {
    if (projectWindow_) {
        return;
    }

    projectWindow_ = new ProjectWindow();
    projectWindow_->hide();
    projectWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectProjectWindowSignals();
}


void UI::ProjectManager::removeProjectWindow() {
    if (!projectWindow_) {
        return;
    }

    removingProjectWindow_ = true;
    projectWindow_->close();
    projectWindow_->deleteLater();
    projectWindow_ = nullptr;
    removingProjectWindow_ = false;
}


void UI::ProjectManager::removeStartWindow() {
    if (!startWindow_) {
        return;
    }

    startWindow_->close();
    startWindow_->deleteLater();
    startWindow_ = nullptr;
}


void UI::ProjectManager::closeTabWindows() {
    const auto windows = tabWindows_;
    tabWindows_.clear();

    for (auto* tabWindow: windows) {
        if (!tabWindow) {
            continue;
        }

        tabWindow->close();
        tabWindow->deleteLater();
    }
}


void UI::ProjectManager::handleProjectWindowClosed() {
    if (removingProjectWindow_) {
        return;
    }

    closeTabWindows();
    tabs_.clear();
    projectData_ = {};

    if (projectWindow_) {
        projectWindow_->deleteLater();
        projectWindow_ = nullptr;
    }
}


void UI::ProjectManager::checkOpenedTabWindow(const QString& tabName) {
    if (tabName.isEmpty()) {
        return;
    }

    if (auto* existingWindow = findTabWindow(tabName)) {
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        return;
    }
    emit openNewWindowTabTriggered(tabName);
}


void UI::ProjectManager::openTabWindow(const QString& tabName,
                                       QWindow* windowRender,
                                       QLineEdit* consoleEngine) {

    auto* tabWindow = new TabWindow(tabName);
    tabWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    tabWindows_.append(tabWindow);

    tabWindow->setQWindowRender(windowRender);
    tabWindow->setCommandConsoleEngine(consoleEngine);

    connect(tabWindow, &TabWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);

    connect(tabWindow, &TabWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);

    connect(tabWindow, &TabWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);

    connect(tabWindow, &TabWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);

    connect(tabWindow, &TabWindow::returnTabTriggered,
            this, [](const QString&) {
            });
    connect(tabWindow, &QObject::destroyed,
            this, [this, tabWindow] {
                tabWindows_.removeOne(tabWindow);
            });

    tabWindow->show();
    tabWindow->raise();
    tabWindow->activateWindow();
}


UI::TabWindow* UI::ProjectManager::findTabWindow(const QString& tabName) const {
    for (auto* tabWindow: tabWindows_) {
        if (tabWindow && tabWindow->tabName() == tabName) {
            return tabWindow;
        }
    }

    return nullptr;
}


UI::BaseWindow* UI::ProjectManager::activeWindow() const {
    if (projectWindow_ && projectWindow_->isVisible()) {
        return projectWindow_;
    }

    return startWindow_;
}


UI::BaseEditorWindow* UI::ProjectManager::activeEditorWindow() const {
    for (auto* tabWindow: tabWindows_) {
        if (tabWindow && tabWindow->isVisible() && tabWindow->isActiveWindow()) {
            return tabWindow;
        }
    }

    if (projectWindow_ && projectWindow_->isVisible()) {
        return projectWindow_;
    }

    return nullptr;
}
