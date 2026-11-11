#include "ProjectManager.h"

#include "BaseEditorWindow.h"
#include "BaseWindow.h"
#include "MainWindow.h"
#include "LightEditorWindow.h"

#include <QCloseEvent>
#include <QLineEdit>
#include <QOpenGLWindow>
#include <QWindow>

#include "MainWindow.h"


UI::ProjectManager::ProjectManager(const EditorContext& context, QObject *parent)
    : QObject(parent)
      , context_(context) {
    ensureMainWindow();

    if (mainWindow_) {
        loadSettings();

        mainWindow_->show();
        mainWindow_->raise();
        mainWindow_->activateWindow();
    }
}


UI::ProjectManager::~ProjectManager() {
    closeLightEditorWindows();

    if (mainWindow_) {
        if (context_.renderEngine) context_.renderEngine->setParent(nullptr);
        if (context_.consoleEngine) context_.consoleEngine->setParent(nullptr);
        mainWindow_->close();
        mainWindow_->deleteLater();
    }

    delete context_.renderEngine;
    delete context_.consoleEngine;
}


void UI::ProjectManager::ensureMainWindow() {
    if (mainWindow_) return;

    mainWindow_ = new MainWindow();
    mainWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectMainWindowSignals();

    if (context_.fileSystem) {
        QVector<ProjectData> data;
        for (const auto& p: context_.fileSystem->projects()) {
            data.append({p.id, p.name, p.path});
        }
        mainWindow_->setFilesList(data);
    }

    if (context_.renderEngine) mainWindow_->setQWindowRender(context_.renderEngine);
    if (context_.consoleEngine) mainWindow_->setCommandConsoleEngine(context_.consoleEngine);
    if (context_.settings) mainWindow_->setSettings(context_.settings);

    mainWindow_->setDefaultFilesPath(context_.fileSystem->projectsPath());
    mainWindow_->setDefaultProjectPath(context_.fileSystem->projectsPath());
    mainWindow_->setDefaultScriptPath(context_.fileSystem->scriptsPath());
}


void UI::ProjectManager::loadSettings() {
    if (context_.settings) {
        const bool notif = context_.settings->isNotificationsEnabled();
        mainWindow_->setNotificationsEnabled(notif);
    }
}


void UI::ProjectManager::addTabInEditor(const FileData& data) {
    ensureMainWindow();
    if (!mainWindow_) return;

    mainWindow_->addTabSlot(data);
    mainWindow_->showEditorPage();

    if (!mainWindow_->isVisible()) mainWindow_->show();
    if (mainWindow_->isMinimized()) mainWindow_->showNormal();

    mainWindow_->raise();
    mainWindow_->activateWindow();
}


void UI::ProjectManager::returnToStartWindow() {
    QVector<ProjectData> data;
    if (context_.fileSystem) {
        for (const auto& p: context_.fileSystem->projects()) {
            data.append({p.id, p.name, p.path});
        }
    }
    showStartWindow(data);
}


void UI::ProjectManager::showStartWindow(const QVector<ProjectData>& data) {
    ensureMainWindow();
    if (!mainWindow_) return;

    closeLightEditorWindows();
    mainWindow_->setFilesList(data);
    mainWindow_->setDefaultFilesPath(context_.fileSystem->projectsPath());
    mainWindow_->showStartPage();

    if (!mainWindow_->isVisible()) mainWindow_->show();
    if (mainWindow_->isMinimized()) mainWindow_->showNormal();

    mainWindow_->raise();
    mainWindow_->activateWindow();
}


// (LightEditor)


void UI::ProjectManager::openLightEditor(const EditorContext& context, const FileData& data) {
    if (auto *existingWindow = findLightEditorWindow(data.id)) {
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        return;
    }

    auto *tabWindow = new LightEditorWindow(data, nullptr);
    tabWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    lightEditorWindows_.append(tabWindow);

    if (context.renderEngine) tabWindow->setQWindowRender(context.renderEngine);
    if (context.consoleEngine) tabWindow->setCommandConsoleEngine(context.consoleEngine);
    if (context.settings) tabWindow->setSettings(context.settings);

    connect(tabWindow, &LightEditorWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);
    connect(tabWindow, &LightEditorWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);
    connect(tabWindow, &LightEditorWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);
    connect(tabWindow, &LightEditorWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);

    connect(tabWindow, &LightEditorWindow::returnTabTriggered,
            this, [this, tabWindow](const qint64 id) {
                emit lightEditorReturned(id);
                lightEditorWindows_.removeOne(tabWindow);
                tabWindow->close();
            });

    connect(tabWindow, &QObject::destroyed,
            this, [this, tabWindow]() {
                emit closeFileTriggered(tabWindow->id());
                lightEditorWindows_.removeOne(tabWindow);
            });

    tabWindow->show();
    tabWindow->raise();
    tabWindow->activateWindow();
}


// signals


void UI::ProjectManager::connectMainWindowSignals() {
    if (!mainWindow_) return;

    connect(mainWindow_, &MainWindow::closeApplicationTriggered,
            this, &ProjectManager::handleEditorWindowClosed);
    connect(mainWindow_, &BaseEditorWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);
    connect(mainWindow_, &BaseEditorWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);
    connect(mainWindow_, &BaseEditorWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);
    connect(mainWindow_, &BaseEditorWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);

    connect(mainWindow_, &MainWindow::openFileByIDTriggered,
            this, &ProjectManager::openFileByIDTriggered);
    connect(mainWindow_, &MainWindow::openFileTriggered,
            this, &ProjectManager::openFileTriggered);
    connect(mainWindow_, &MainWindow::createFileTriggered,
            this, &ProjectManager::createFileTriggered);
    connect(mainWindow_, &MainWindow::renameFileTriggered,
            this, &ProjectManager::renameFileTriggered);
    connect(mainWindow_, &MainWindow::deleteFileTriggered,
            this, &ProjectManager::removeFileTriggered);

    connect(mainWindow_, &MainWindow::goToStartWindowTriggered,
            this, &ProjectManager::returnToStartWindow);
    connect(mainWindow_, &MainWindow::renameTabTriggered,
            this, &ProjectManager::renameFileTriggered);
    connect(mainWindow_, &MainWindow::removeTabTriggered,
            this, &ProjectManager::removeFileTriggered);
    connect(mainWindow_, &MainWindow::closeTabTriggered,
            this, &ProjectManager::closeFileTriggered);
    connect(mainWindow_, &MainWindow::setActiveTabTriggered,
            this, &ProjectManager::setActiveFileTriggered);
    connect(mainWindow_, &MainWindow::openLightEditorWindowTriggered,
            this, &ProjectManager::lightEditorRequested);
    connect(mainWindow_, &MainWindow::onOpenScriptTriggered,
            this, &ProjectManager::scriptTriggered);
    connect(mainWindow_, &MainWindow::settingsChanged,
            this, &ProjectManager::settingsChanged);

    connect(mainWindow_, &MainWindow::settingsChanged, this, [this]() {
        if (context_.settings)
            mainWindow_->setNotificationsEnabled(context_.settings->isNotificationsEnabled());
        emit settingsChanged();
    });
}


void UI::ProjectManager::handleEditorWindowClosed() {
    closeLightEditorWindows();
    emit saveFullProjectTriggered();
}


void UI::ProjectManager::closeLightEditorWindows() {
    const auto windows = lightEditorWindows_;
    lightEditorWindows_.clear();

    for (auto *window: windows) {
        if (!window) continue;
        window->close();
        window->deleteLater();
    }
}


void UI::ProjectManager::deleteTabSlot(const qint64 id) {
    if (mainWindow_) mainWindow_->deleteTabSlot(id);

    if (auto *tabWindow = findLightEditorWindow(id)) {
        lightEditorWindows_.removeOne(tabWindow);
        tabWindow->close();
        tabWindow->deleteLater();
    }
}


void UI::ProjectManager::renameTabSlot(FileData& data) {
    if (mainWindow_) mainWindow_->renameTabSlot(data);
    if (auto *tabWindow = findLightEditorWindow(data.id)) {
        tabWindow->setName(data.name);
    }
}


void UI::ProjectManager::setActiveTab(const qint64 id) {
    if (mainWindow_) mainWindow_->setActiveID(id);
}


// Notification


void UI::ProjectManager::addNotification(const qint64 id, const QString& text) const {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->addNotification(text);
            return;
        }
    }
    if (const auto *window = activeWindow()) window->addNotification(text);
}


void UI::ProjectManager::addNotification(const QString& text) const {
    if (const auto *window = activeWindow()) window->addNotification(text);
}


UI::LightEditorWindow* UI::ProjectManager::findLightEditorWindow(const qint64 id) const {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) return window;
    }
    return nullptr;
}


UI::BaseWindow* UI::ProjectManager::activeWindow() const {
    return mainWindow_;
}


//  (EditorWindow)


void UI::ProjectManager::setActiveToolEditorWindow(ToolsType tool) const {
    if (mainWindow_) mainWindow_->setActiveTool(tool);
}


void UI::ProjectManager::setActiveToolEditorWindow(PrimitiveType tool) const {
    if (mainWindow_) mainWindow_->setActiveTool(tool);
}


void UI::ProjectManager::setActiveToolEditorWindow(ConstraintType tool) {
    if (mainWindow_) mainWindow_->setActiveTool(tool);
}


void UI::ProjectManager::setHintConstraintToolsEditorWindow(const QVector<ConstraintType>& vecTools) {
    if (mainWindow_) mainWindow_->setHintConstraintTools(vecTools);
}


void UI::ProjectManager::takeOffHintConstraintToolsEditorWindow() {
    if (mainWindow_) mainWindow_->takeOffHint();
}


//  (LightEditorWindow)


void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, ConstraintType tool) {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}


void UI::ProjectManager::takeOffHintConstraintToolsLightEditorWindow(const qint64 id) {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->takeOffHint();
            return;
        }
    }
}


void UI::ProjectManager::setHintConstraintToolsLightEditorWindow(const qint64 id,
                                                                 const QVector<ConstraintType>& vecTools) {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setHintConstraintTools(vecTools);
            return;
        }
    }
}


void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, ToolsType tool) {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}


void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, PrimitiveType tool) {
    for (auto *window: lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}


void UI::ProjectManager::closeEvent(QCloseEvent *event) {
    emit saveFullProjectTriggered();
    if (event) event->accept();
}
