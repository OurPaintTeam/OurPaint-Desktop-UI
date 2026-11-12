#include "ProjectManager.h"

#include "BaseEditorWindow.h"
#include "BaseWindow.h"
#include "MainWindow.h"
#include "LightEditorWindow.h"

#include <QCloseEvent>
#include <QLineEdit>
#include <QOpenGLWindow>
#include <QWindow>

UI::ProjectManager::ProjectManager(const EditorContext& context, QObject* parent)
    : QObject(parent)
    , context_(context) {
}

UI::ProjectManager::~ProjectManager() {
    if (mainWindow_) {
        mainWindow_->deleteLater();
        mainWindow_ = nullptr;
    }

    if (lightEditorWindow_) {
        lightEditorWindow_->deleteLater();
        lightEditorWindow_ = nullptr;
    }
}

void UI::ProjectManager::showStartWindow() {
    ensureMainWindow();
    if (mainWindow_) {
        loadSettings();
        mainWindow_->show();
        mainWindow_->raise();
        mainWindow_->activateWindow();
    }
}

void UI::ProjectManager::addTabSlot(FileData& data) {
    ensureMainWindow();
    if (mainWindow_) {
        mainWindow_->addTabSlot(data);
        if (!mainWindow_->isVisible()) mainWindow_->show();
        if (mainWindow_->isMinimized()) mainWindow_->showNormal();
        mainWindow_->raise();
        mainWindow_->activateWindow();
    }
}

void UI::ProjectManager::showEditorWindow(FileData& data) {
    ensureMainWindow();
    if (mainWindow_) {
        mainWindow_->addTabSlot(data);
        mainWindow_->showEditorPage();

        loadSettings();
        if (!mainWindow_->isVisible()) mainWindow_->show();
        if (mainWindow_->isMinimized()) mainWindow_->showNormal();
        mainWindow_->raise();
        mainWindow_->activateWindow();
    }
}

void UI::ProjectManager::showLightEditorWindow(const FileData& data) {
    ensureLightEditorWindow(data);
    if (lightEditorWindow_) {
        loadSettings();
        lightEditorWindow_->show();
        lightEditorWindow_->raise();
        lightEditorWindow_->activateWindow();
    }
}

bool UI::ProjectManager::isLightEditor() const {
    return lightEditorWindow_ != nullptr;
}

void UI::ProjectManager::ensureLightEditorWindow(const FileData& data) {
    if (lightEditorWindow_) return;

    lightEditorWindow_ = new LightEditorWindow(data, nullptr);
    lightEditorWindow_->setAttribute(Qt::WA_DeleteOnClose, true);

    if (context_.renderEngine) lightEditorWindow_->setQWindowRender(context_.renderEngine);
    if (context_.consoleEngine) lightEditorWindow_->setCommandConsoleEngine(context_.consoleEngine);
    if (context_.settings) lightEditorWindow_->setSettings(context_.settings);

    connect(lightEditorWindow_, &LightEditorWindow::sentCommandTriggered, this, &ProjectManager::sentCommandTriggered);
    connect(lightEditorWindow_, &LightEditorWindow::primitiveTriggered, this, &ProjectManager::primitiveTriggered);
    connect(lightEditorWindow_, &LightEditorWindow::constraintTriggered, this, &ProjectManager::constraintTriggered);
    connect(lightEditorWindow_, &LightEditorWindow::toolsTriggered, this, &ProjectManager::toolsTriggered);

    connect(lightEditorWindow_, &LightEditorWindow::returnTabTriggered, this, [this](const qint64 id) {
        emit returnTabToEditorRequested(id);
        lightEditorWindow_->close();
    });

    connect(lightEditorWindow_, &QObject::destroyed, this, [this]() {
        emit closeLightEditorWindowTriggered(lightEditorWindow_->id());
        lightEditorWindow_ = nullptr;

        context_.renderEngine = nullptr;
        context_.consoleEngine = nullptr;

        deleteLater();
    });

    lightEditorWindow_->move(QCursor::pos());
}

void UI::ProjectManager::ensureMainWindow() {
    if (mainWindow_) return;

    mainWindow_ = new MainWindow();
    mainWindow_->setAttribute(Qt::WA_DeleteOnClose, false);
    connectMainWindowSignals();

    if (context_.fileSystem) {
        QVector<ProjectData> data;
        for (const auto& p : context_.fileSystem->projects()) {
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

void UI::ProjectManager::loadSettings() const {
    if (context_.settings) {
        const bool notif = context_.settings->isNotificationsEnabled();
        if (mainWindow_) mainWindow_->setNotificationsEnabled(notif);
        if (lightEditorWindow_) lightEditorWindow_->setNotificationsEnabled(notif);
    }
}

void UI::ProjectManager::connectMainWindowSignals() {
    if (!mainWindow_) return;

    connect(mainWindow_, &MainWindow::closeApplicationTriggered, this, &ProjectManager::handleEditorWindowClosed);
    connect(mainWindow_, &BaseEditorWindow::sentCommandTriggered, this, &ProjectManager::sentCommandTriggered);
    connect(mainWindow_, &BaseEditorWindow::primitiveTriggered, this, &ProjectManager::primitiveTriggered);
    connect(mainWindow_, &BaseEditorWindow::constraintTriggered, this, &ProjectManager::constraintTriggered);
    connect(mainWindow_, &BaseEditorWindow::toolsTriggered, this, &ProjectManager::toolsTriggered);

    connect(mainWindow_, &MainWindow::openFileByIDTriggered, this, &ProjectManager::openFileByIDTriggered);
    connect(mainWindow_, &MainWindow::openFileTriggered, this, &ProjectManager::openFileTriggered);
    connect(mainWindow_, &MainWindow::createFileTriggered, this, &ProjectManager::createFileTriggered);
    
    connect(mainWindow_, &MainWindow::renameFileTriggered, this, [this](UI::FileData& data) {
        emit renameFileTriggered(data);
        if (context_.fileSystem) {
            QVector<ProjectData> list;
            for (const auto& p : context_.fileSystem->projects()) {
                list.append({p.id, p.name, p.path});
            }
            mainWindow_->setFilesList(list);
        }
    });

    connect(mainWindow_, &MainWindow::deleteFileTriggered, this, [this](qint64 id) {
        emit removeFileTriggered(id);
        if (context_.fileSystem) {
            QVector<ProjectData> list;
            for (const auto& p : context_.fileSystem->projects()) {
                list.append({p.id, p.name, p.path});
            }
            mainWindow_->setFilesList(list);
        }
    });

    connect(mainWindow_, &MainWindow::goToStartWindowTriggered, this, &ProjectManager::returnToStartWindow);
    connect(mainWindow_, &MainWindow::renameTabTriggered, this, &ProjectManager::renameFileTriggered);
    connect(mainWindow_, &MainWindow::removeTabTriggered, this, &ProjectManager::removeFileTriggered);
    connect(mainWindow_, &MainWindow::closeTabTriggered, this, &ProjectManager::closeFileTriggered);
    connect(mainWindow_, &MainWindow::setActiveTabTriggered, this, &ProjectManager::setActiveFileTriggered);
    connect(mainWindow_, &MainWindow::openLightEditorWindowTriggered, this, &ProjectManager::lightEditorRequested);
    connect(mainWindow_, &MainWindow::onOpenScriptTriggered, this, &ProjectManager::scriptTriggered);
    connect(mainWindow_, &MainWindow::tabMovedToAnotherWindow, this, &ProjectManager::tabMovedToAnotherWindow);

    connect(mainWindow_, &MainWindow::settingsChanged, this, [this]() {
        if (context_.settings) {
            const bool notif = context_.settings->isNotificationsEnabled();
            mainWindow_->setNotificationsEnabled(notif);
            if (lightEditorWindow_) lightEditorWindow_->setNotificationsEnabled(notif);
        }
        emit settingsChanged();
    });
}

void UI::ProjectManager::handleEditorWindowClosed() {
    emit saveFullProjectTriggered();
}

void UI::ProjectManager::closeTabSlot(const qint64 id) {
    if (mainWindow_) mainWindow_->closeTabSlot(id);
    if (lightEditorWindow_ && lightEditorWindow_->id() == id) lightEditorWindow_->close();
}

void UI::ProjectManager::deleteTabSlot(const qint64 id) {
    if (mainWindow_) mainWindow_->deleteTabSlot(id);
    if (lightEditorWindow_ && lightEditorWindow_->id() == id) lightEditorWindow_->close();
}

void UI::ProjectManager::renameTabSlot(FileData& data) {
    if (mainWindow_) mainWindow_->renameTabSlot(data);
    if (lightEditorWindow_ && lightEditorWindow_->id() == data.id) lightEditorWindow_->setName(data.name);
}

void UI::ProjectManager::setActiveTab(const qint64 id) {
    if (mainWindow_) mainWindow_->setActiveID(id);
}

void UI::ProjectManager::addNotification(const qint64 id, const QString& text) const {
    if (lightEditorWindow_ && lightEditorWindow_->id() == id) {
        lightEditorWindow_->addNotification(text);
        return;
    }
    if (mainWindow_ && mainWindow_->getActiveTabId() == id) {
        mainWindow_->addNotification(text);
    }
}

void UI::ProjectManager::addNotification(const QString& text) const {
    if (lightEditorWindow_) {
        lightEditorWindow_->addNotification(text);
        return;
    }
    if (mainWindow_) {
        mainWindow_->addNotification(text);
    }
}

UI::BaseWindow* UI::ProjectManager::activeWindow() const {
    if (mainWindow_) return mainWindow_;
    return lightEditorWindow_;
}

QWidget* UI::ProjectManager::windowWidget() const {
    if (mainWindow_) return mainWindow_;
    if (lightEditorWindow_) return lightEditorWindow_;
    return nullptr;
}

void UI::ProjectManager::setActiveTool(ToolsType tool) const {
    if (mainWindow_) {
        mainWindow_->setActiveTool(tool);
    } else if (lightEditorWindow_) {
        lightEditorWindow_->setActiveTool(tool);
    }
}

void UI::ProjectManager::setActiveTool(PrimitiveType tool) const {
    if (mainWindow_) {
        mainWindow_->setActiveTool(tool);
    } else if (lightEditorWindow_) {
        lightEditorWindow_->setActiveTool(tool);
    }
}

void UI::ProjectManager::setActiveTool(ConstraintType tool) const {
    if (mainWindow_) {
        mainWindow_->setActiveTool(tool);
    } else if (lightEditorWindow_) {
        lightEditorWindow_->setActiveTool(tool);
    }
}

void UI::ProjectManager::setHintConstraintTools(const QVector<ConstraintType>& vecTools) const {
    if (mainWindow_) {
        mainWindow_->setHintConstraintTools(vecTools);
    } else if (lightEditorWindow_) {
        lightEditorWindow_->setHintConstraintTools(vecTools);
    }
}

void UI::ProjectManager::takeOffHintConstraintTools() const {
    if (mainWindow_) {
        mainWindow_->takeOffHint();
    } else if (lightEditorWindow_) {
        lightEditorWindow_->takeOffHint();
    }
}

void UI::ProjectManager::returnToStartWindow() {
    QVector<ProjectData> data;
    if (context_.fileSystem) {
        for (const auto& p : context_.fileSystem->projects()) {
            data.append({p.id, p.name, p.path});
        }
    }
    showStartWindow(data);
}

void UI::ProjectManager::showStartWindow(const QVector<ProjectData>& data) {
    ensureMainWindow();
    if (!mainWindow_) return;

    mainWindow_->setFilesList(data);
    mainWindow_->setDefaultFilesPath(context_.fileSystem->projectsPath());
    mainWindow_->showStartPage();

    if (!mainWindow_->isVisible()) mainWindow_->show();
    if (mainWindow_->isMinimized()) mainWindow_->showNormal();

    mainWindow_->raise();
    mainWindow_->activateWindow();
}