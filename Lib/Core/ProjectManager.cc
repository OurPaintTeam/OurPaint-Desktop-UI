#include "ProjectManager.h"

#include "BaseEditorWindow.h"
#include "BaseWindow.h"
#include "EditorWindow.h"
#include "LightEditorWindow.h"
#include "StartWindow.h"

#include <QCloseEvent>
#include <QLineEdit>
#include <QOpenGLWindow>
#include <QWindow>


UI::ProjectManager::ProjectManager(const EditorContext& context, QObject* parent)
    : QObject(parent)
    , context_(context)
{

    ensureStartWindow();
    startWindow_->show();
    startWindow_->raise();
    startWindow_->activateWindow();
}

UI::ProjectManager::~ProjectManager() {
    closeLightEditorWindows();

    if (editorWindow_) {
        if (context_.renderEngine) context_.renderEngine->setParent(nullptr);
        if (context_.consoleEngine) context_.consoleEngine->setParent(nullptr);

        editorWindow_->close();
        editorWindow_->deleteLater();
    }

    if (startWindow_) {
        startWindow_->close();
        startWindow_->deleteLater();
    }

    delete context_.renderEngine;
    delete context_.consoleEngine;
}


// init


void UI::ProjectManager::ensureStartWindow() {
    if (startWindow_) return;

    startWindow_ = new StartWindow();
    startWindow_->hide();
    startWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectStartWindowSignals();

    if (context_.fileSystem) {
        QVector<ProjectData> data;
        for (const auto& p : context_.fileSystem->projects()) {
            data.append({p.id, p.name, p.path});
        }
        startWindow_->setFilesList(data);
    }

    startWindow_->setDefaultFilesPath(context_.defaultProjectPath);
}

void UI::ProjectManager::ensureEditorWindow() {
    if (editorWindow_) return;

    editorWindow_ = new EditorWindow(nullptr);
    editorWindow_->hide();
    editorWindow_->setAttribute(Qt::WA_DeleteOnClose, false);


    if (context_.renderEngine) editorWindow_->setQWindowRender(context_.renderEngine);
    if (context_.consoleEngine) editorWindow_->setCommandConsoleEngine(context_.consoleEngine);
    if (context_.settingsSystem) editorWindow_->setSettings(context_.settingsSystem);

    editorWindow_->setDefaultProjectPath(context_.defaultProjectPath);
    editorWindow_->setDefaultScriptPath(context_.defaultScriptPath);

    connectEditorWindowSignals();
}


// state


void UI::ProjectManager::addTabInEditor(const FileData& data) {
    if (startWindow_) {
        lastWindowGeometry_ = startWindow_->saveGeometry();
        startWindow_->close();
        startWindow_->deleteLater();
        startWindow_ = nullptr;
    }

    bool isNewWindow = (editorWindow_ == nullptr);

    ensureEditorWindow();

    if (editorWindow_) {
        editorWindow_->addTabSlot(data);

        if (isNewWindow && !lastWindowGeometry_.isEmpty()) {
            editorWindow_->restoreGeometry(lastWindowGeometry_);
        }


        if (isNewWindow || !editorWindow_->isVisible()) {
            editorWindow_->show();
        }

        if (editorWindow_->isMinimized()) {
            editorWindow_->showNormal();
        }

        editorWindow_->raise();
        editorWindow_->activateWindow();
    }
}

void UI::ProjectManager::returnToStartWindow() {
    if (startWindow_ && startWindow_->isVisible()) {
        return;
    }


    QVector<ProjectData> data;
    if (context_.fileSystem) {
        for (const auto& p : context_.fileSystem->projects()) {
            data.append({p.id, p.name, p.path});
        }
    }

    showStartWindow(data);
}

void UI::ProjectManager::showStartWindow(const QVector<ProjectData>& data) {
    if (editorWindow_) {
        lastWindowGeometry_ = editorWindow_->saveGeometry();
        removingEditorWindow_ = true;

        if (context_.renderEngine) {
            context_.renderEngine->setParent(nullptr);
            context_.renderEngine->hide();
        }
        if (context_.consoleEngine) {
            context_.consoleEngine->setParent(nullptr);
            context_.consoleEngine->hide();
        }

        editorWindow_->close();
        editorWindow_->deleteLater();
        editorWindow_ = nullptr;
        removingEditorWindow_ = false;
    }

    closeLightEditorWindows();
    ensureStartWindow();

    startWindow_->setFilesList(data);
    startWindow_->setDefaultFilesPath(context_.defaultProjectPath);

    if (!lastWindowGeometry_.isEmpty()) {
        startWindow_->restoreGeometry(lastWindowGeometry_);
    }

    startWindow_->show();
    startWindow_->raise();
    startWindow_->activateWindow();
}


// (LightEditor)


void UI::ProjectManager::openLightEditor(const EditorContext& context, const FileData& data) {
    if (auto *existingWindow = findLightEditorWindow(data.id)) {
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        return;
    }

    auto *tabWindow = new LightEditorWindow(data,nullptr);
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


void UI::ProjectManager::connectStartWindowSignals() {
    if (!startWindow_) return;

    connect(startWindow_, &StartWindow::openFileByIDTriggered,
            this, &ProjectManager::openFileByIDTriggered);
    connect(startWindow_, &StartWindow::openFileTriggered,
            this, &ProjectManager::openFileTriggered);
    connect(startWindow_, &StartWindow::createFileTriggered,
            this, &ProjectManager::createFileTriggered);
    connect(startWindow_, &StartWindow::renameFileTriggered,
            this, &ProjectManager::renameFileTriggered);
    connect(startWindow_, &StartWindow::deleteFileTriggered,
            this, &ProjectManager::removeFileTriggered);
}

void UI::ProjectManager::connectEditorWindowSignals() {
    if (!editorWindow_) return;

    connect(editorWindow_, &EditorWindow::closeApplicationTriggered,
            this, &ProjectManager::handleEditorWindowClosed);

    connect(editorWindow_, &EditorWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);
    connect(editorWindow_, &EditorWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);
    connect(editorWindow_, &EditorWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);
    connect(editorWindow_, &EditorWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);

    // start
    connect(editorWindow_, &EditorWindow::goToStartWindowTriggered,
            this, &ProjectManager::returnToStartWindow);

    // Files
    connect(editorWindow_, &EditorWindow::openFileTriggered,
            this, &ProjectManager::openFileTriggered);
    connect(editorWindow_, &EditorWindow::createFileTriggered,
            this, &ProjectManager::createFileTriggered);
    connect(editorWindow_, &EditorWindow::renameTabTriggered,
            this, &ProjectManager::renameFileTriggered);
    connect(editorWindow_, &EditorWindow::removeTabTriggered,
            this, &ProjectManager::removeFileTriggered);
    connect(editorWindow_, &EditorWindow::closeTabTriggered,
            this, &ProjectManager::closeFileTriggered);
    connect(editorWindow_, &EditorWindow::setActiveTabTriggered,
            this, &ProjectManager::setActiveFileTriggered);


    connect(editorWindow_, &EditorWindow::openLightEditorWindowTriggered,
            this, &ProjectManager::lightEditorRequested);


    connect(editorWindow_, &EditorWindow::onOpenScriptTriggered,
            this, &ProjectManager::scriptTriggered);
    connect(editorWindow_, &EditorWindow::settingsChanged,
            this, &ProjectManager::settingsChanged);
}


// Slots


void UI::ProjectManager::handleEditorWindowClosed() {
    if (removingEditorWindow_) return;

    closeLightEditorWindows();

    if (editorWindow_) {
        editorWindow_->deleteLater();
        editorWindow_ = nullptr;
    }

    emit saveFullProjectTriggered();
}

void UI::ProjectManager::closeLightEditorWindows() {
    const auto windows = lightEditorWindows_;
    lightEditorWindows_.clear();

    for (auto *window : windows) {
        if (!window) continue;
        window->close();
        window->deleteLater();
    }
}

void UI::ProjectManager::deleteTabSlot(const qint64 id) {
    if (editorWindow_) editorWindow_->deleteTabSlot(id);

    if (auto *tabWindow = findLightEditorWindow(id)) {
        lightEditorWindows_.removeOne(tabWindow);
        tabWindow->close();
        tabWindow->deleteLater();
    }
}

void UI::ProjectManager::renameTabSlot(FileData& data) {
    if (editorWindow_) editorWindow_->renameTabSlot(data);
    if (auto *tabWindow = findLightEditorWindow(data.id)) {
        tabWindow->setName(data.name);
    }
}

void UI::ProjectManager::setActiveTab(const qint64 id) {
    if (editorWindow_) editorWindow_->setActiveID(id);
}


// Notification


void UI::ProjectManager::addNotification(const qint64 id, const QString& text) const {
    for (auto *window : lightEditorWindows_) {
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
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) return window;
    }
    return nullptr;
}

UI::BaseWindow* UI::ProjectManager::activeWindow() const {
    if (editorWindow_ && editorWindow_->isVisible()) return editorWindow_;
    return startWindow_;
}


//  (EditorWindow)


void UI::ProjectManager::setActiveToolEditorWindow(ToolsType tool) const {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setActiveToolEditorWindow(PrimitiveType tool) const {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setActiveToolEditorWindow(ConstraintType tool) {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setHintConstraintToolsEditorWindow(const QVector<ConstraintType>& vecTools) {
    if (editorWindow_) editorWindow_->setHintConstraintTools(vecTools);
}

void UI::ProjectManager::takeOffHintConstraintToolsEditorWindow() {
    if (editorWindow_) editorWindow_->takeOffHint();
}


//  (LightEditorWindow)


void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, ConstraintType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) { window->setActiveTool(tool); return; }
    }
}

void UI::ProjectManager::takeOffHintConstraintToolsLightEditorWindow(const qint64 id) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) { window->takeOffHint(); return; }
    }
}

void UI::ProjectManager::setHintConstraintToolsLightEditorWindow(const qint64 id, const QVector<ConstraintType>& vecTools) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) { window->setHintConstraintTools(vecTools); return; }
    }
}

void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, ToolsType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) { window->setActiveTool(tool); return; }
    }
}

void UI::ProjectManager::setActiveToolLightEditorWindow(const qint64 id, PrimitiveType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) { window->setActiveTool(tool); return; }
    }
}



void UI::ProjectManager::closeEvent(QCloseEvent *event) {
    emit saveFullProjectTriggered();
    if (event) event->accept();
}