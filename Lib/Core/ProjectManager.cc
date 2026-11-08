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

UI::ProjectManager::ProjectManager() {
    // For this constructor we always start with StartWindow.
    ensureStartWindow();
    openStartWindow();
}

UI::ProjectManager::ProjectManager(QWindow *windowRender, QLineEdit *consoleEngine, SettingsSystem* settingsSystem) {
    // No StartWindow. Create EditorWindow and apply engines.
    reOpenNewWindow();

    if (windowRender) {
        setQWindowRender(windowRender);
    }
    if (consoleEngine) {
        setCommandConsoleEngine(consoleEngine);
    }
    if (settingsSystem) {
        setSettings(settingsSystem);
    }
}

UI::ProjectManager::~ProjectManager() {
    closeLightEditorWindows();

    if (editorWindow_) {
        editorWindow_->close();
        editorWindow_->deleteLater();
    }
    if (startWindow_) {
        startWindow_->close();
        startWindow_->deleteLater();
    }
}

void UI::ProjectManager::setActiveToolEditorWindow(ToolsType tool) const {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setActiveToolEditorWindow(PrimitiveType tool) const {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setActiveToolEditorWindow(ConstraintType tool) {
    if (editorWindow_) editorWindow_->setActiveTool(tool);
}

void UI::ProjectManager::setHintConstraintToolsEditorWindow(
    const QVector<ConstraintType>& vecTools) {
    if (editorWindow_) editorWindow_->setHintConstraintTools(vecTools);
}

void UI::ProjectManager::takeOffHintConstraintToolsEditorWindow() {
    if (editorWindow_) editorWindow_->takeOffHint();
}

void UI::ProjectManager::setActiveToolLightEditorWindow(
    const qint64 id, ConstraintType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}

void UI::ProjectManager::takeOffHintConstraintToolsLightEditorWindow(
    const qint64 id) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) {
            window->takeOffHint();
            return;
        }
    }
}

void UI::ProjectManager::setHintConstraintToolsLightEditorWindow(
    const qint64 id, const QVector<ConstraintType>& vecTools) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setHintConstraintTools(vecTools);
            return;
        }
    }
}

void UI::ProjectManager::setActiveToolLightEditorWindow(
   const qint64 id, ToolsType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}

void UI::ProjectManager::setActiveToolLightEditorWindow(
    const qint64 id, PrimitiveType tool) {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) {
            window->setActiveTool(tool);
            return;
        }
    }
}

void UI::ProjectManager::setActiveTab(const qint64 id) {
    if (editorWindow_) editorWindow_->setActiveID(id);
}

void UI::ProjectManager::setFilesList(
    const QVector<FileData>& files) {
    filesList_ = files;
    if (startWindow_) startWindow_->setFilesList(filesList_);
}

void UI::ProjectManager::addNotification(
    const qint64 id, const QString& text) const {
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

void UI::ProjectManager::openStartWindow() {
    closeLightEditorWindows();
    removeEditorWindow();
    ensureStartWindow();

     startWindow_->setFilesList(filesList_);

    startWindow_->show();
    startWindow_->raise();
    startWindow_->activateWindow();
}

void UI::ProjectManager::reOpenNewWindow() {
    removeStartWindow();
    closeLightEditorWindows();
    removeEditorWindow();
    ensureEditorWindow();

    editorWindow_->show();
    editorWindow_->raise();
    editorWindow_->activateWindow();
}
//
// void UI::ProjectManager::openNewWindow(
//     QWindow *windowRender,
//     QLineEdit *consoleEngine,
//     SettingsSystem* settings) {
//     reOpenNewWindow();
//     if (windowRender) setQWindowRender(windowRender);
//     if (consoleEngine) setCommandConsoleEngine(consoleEngine);
//     if (settings) setSettings(settings);
// }

void UI::ProjectManager::openLightEditorWindow(
   const QString& fileName,const qint64 id,
   QWindow *windowRender,
   QLineEdit *consoleEngine) {

    if (auto *existingWindow = findTabWindow(id)) {
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        return;
    }

    auto *tabWindow = new LightEditorWindow(id,fileName);
    tabWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    lightEditorWindows_.append(tabWindow);

    if (windowRender) tabWindow->setQWindowRender(windowRender);
    if (consoleEngine) tabWindow->setCommandConsoleEngine(consoleEngine);

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
                emit returnLightEditorWindowTriggered(id);
                lightEditorWindows_.removeOne(tabWindow);
                tabWindow->close();
            });

    connect(tabWindow, &QObject::destroyed,
            this, [this, tabWindow]() {
                lightEditorWindows_.removeOne(tabWindow);
            });

    tabWindow->show();
    tabWindow->raise();
    tabWindow->activateWindow();
}

void UI::ProjectManager::checkOpenedNewWindow(const qint64 id) {

    if (auto *existingWindow = findTabWindow(id)) {
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        return;
    }
    emit openLightEditorTriggered(id);
}

void UI::ProjectManager::addTabSlot(const QString& name,const qint64 id) {
    if (!name.isEmpty() && editorWindow_) editorWindow_->addTabSlot(name,id);
}

void UI::ProjectManager::deleteTabSlot(const qint64 id) {


    if (editorWindow_) editorWindow_->deleteTabSlot(id);

    if (auto *tabWindow = findTabWindow(id)) {
        lightEditorWindows_.removeOne(tabWindow);
        tabWindow->close();
        tabWindow->deleteLater();
    }
}

void UI::ProjectManager::renameTabSlot(
    const qint64 id, const QString& newName) {
    if (newName.isEmpty()) return;

    if (editorWindow_) editorWindow_->renameTabSlot(id, newName);

    if (auto *tabWindow = findTabWindow(id)) {
        tabWindow->setName(newName);
    }
}

void UI::ProjectManager::setQOpenGLPainter(QOpenGLWindow *engine) const {
    if (const auto *window = activeEditorWindow()) window->setQOpenGLPainter(engine);
}

void UI::ProjectManager::setQWindowRender(QWindow *engine) const {
    if (!engine) return;
    if (const auto *window = activeEditorWindow()) window->setQWindowRender(engine);
}

void UI::ProjectManager::setCommandConsoleEngine(QLineEdit *engine) const {
    if (const auto *window = activeEditorWindow()) window->setCommandConsoleEngine(engine);
}

void UI::ProjectManager::setSettings(SettingsSystem *settings) const {
    if (const auto *window = activeEditorWindow()) window->setSettings(settings);
}

void UI::ProjectManager::closeEvent(QCloseEvent *event) {
    emit saveFullProjectTriggered();
    if (event) event->accept();
}

void UI::ProjectManager::connectStartWindowSignals() {
    if (!startWindow_) return;

    connect(startWindow_, &StartWindow::openFileThisWindowTriggered,
            this, &ProjectManager::openFileThisWindowTriggered);
    connect(startWindow_, &StartWindow::createFileThisWindowTriggered,
            this, &ProjectManager::createFileThisWindowTriggered);
    connect(startWindow_, &StartWindow::renameFileTriggered,
            this, &ProjectManager::renameFileTriggered);
    connect(startWindow_, &StartWindow::deleteFileTriggered,
            this, &ProjectManager::deleteFileTriggered);
}

void UI::ProjectManager::connectEditorWindowSignals() {
    if (!editorWindow_) return;

    connect(editorWindow_, &EditorWindow::windowClosedTriggered,
            this, &ProjectManager::handleEditorWindowClosed);
    connect(editorWindow_, &EditorWindow::sentCommandTriggered,
            this, &ProjectManager::sentCommandTriggered);
    connect(editorWindow_, &EditorWindow::primitiveTriggered,
            this, &ProjectManager::primitiveTriggered);
    connect(editorWindow_, &EditorWindow::constraintTriggered,
            this, &ProjectManager::constraintTriggered);
    connect(editorWindow_, &EditorWindow::toolsTriggered,
            this, &ProjectManager::toolsTriggered);
    connect(editorWindow_, &EditorWindow::goToStartWindowTriggered,
            this, &ProjectManager::goToStartWindowTriggered);
    connect(editorWindow_, &EditorWindow::openFileThisWindowTriggered,
            this, &ProjectManager::openFileThisWindowTriggered);
    connect(editorWindow_, &EditorWindow::createFileThisWindowTriggered,
            this, &ProjectManager::createFileThisWindowTriggered);
    connect(editorWindow_, &EditorWindow::openNewWindowOpenFileTriggered,
            this, &ProjectManager::openNewWindowOpenFileTriggered);
    connect(editorWindow_, &EditorWindow::openNewWindowCreateFileTriggered,
            this, &ProjectManager::openNewWindowCreateFileTriggered);
    connect(editorWindow_, &EditorWindow::openFileTriggered,
            this, &ProjectManager::openFileTriggered);
    connect(editorWindow_, &EditorWindow::createFileTriggered,
            this, &ProjectManager::createFileTriggered);
    connect(editorWindow_, &EditorWindow::renameTabTriggered,
            this, &ProjectManager::renameTabTriggered);
    connect(editorWindow_, &EditorWindow::removeTabTriggered,
            this, &ProjectManager::removeTabTriggered);
    connect(editorWindow_, &EditorWindow::openLightEditorWindowTriggered,
            this, &ProjectManager::checkOpenedNewWindow);
    connect(editorWindow_, &EditorWindow::setActiveTabTriggered,
            this, &ProjectManager::setActiveTabTriggered);
    connect(editorWindow_, &EditorWindow::onOpenScriptTriggered,
            this, &ProjectManager::scriptTriggered);

    connect(editorWindow_, &EditorWindow::closeApplicationTriggered,
            this, [this]() {
                if (auto *window = activeWindow()) window->close();
            });

    connect(editorWindow_, &EditorWindow::settingsChanged,
            this, &ProjectManager::settingsChanged);
}

void UI::ProjectManager::ensureStartWindow() {
    if (startWindow_) return;

    startWindow_ = new StartWindow();
    startWindow_->hide();
    startWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectStartWindowSignals();
}

void UI::ProjectManager::ensureEditorWindow() {
    if (editorWindow_) return;

    editorWindow_ = new EditorWindow(nullptr);
    editorWindow_->hide();
    editorWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectEditorWindowSignals();
}

void UI::ProjectManager::removeEditorWindow() {
    if (!editorWindow_) return;

    removingEditorWindow_ = true;
    editorWindow_->close();
    editorWindow_->deleteLater();
    editorWindow_ = nullptr;
    removingEditorWindow_ = false;
}

void UI::ProjectManager::removeStartWindow() {
    if (!startWindow_) return;

    startWindow_->close();
    startWindow_->deleteLater();
    startWindow_ = nullptr;
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

void UI::ProjectManager::handleEditorWindowClosed() {
    if (removingEditorWindow_) return;

    closeLightEditorWindows();

    if (editorWindow_) {
        editorWindow_->deleteLater();
        editorWindow_ = nullptr;
    }
}

UI::LightEditorWindow* UI::ProjectManager::findTabWindow(
    const qint64 id) const {
    for (auto *window : lightEditorWindows_) {
        if (window && window->id() == id) return window;
    }
    return nullptr;
}

UI::BaseWindow* UI::ProjectManager::activeWindow() const {
    if (editorWindow_ && editorWindow_->isVisible()) return editorWindow_;
    return startWindow_;
}

UI::BaseEditorWindow* UI::ProjectManager::activeEditorWindow() const {
    for (auto *window : lightEditorWindows_) {
        if (window && window->isVisible() && window->isActiveWindow()) return window;
    }

    if (editorWindow_ && editorWindow_->isVisible()) return editorWindow_;
    return nullptr;
}
