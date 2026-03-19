#include "UIManager.h"

#include <QFileInfo>
#include <QPointer>

#include "CadViewportWindow.h"
#include "MainWindow.h"


UI::UIManager::UIManager() {
    setlocale(LC_ALL, "");
    const auto* window = createWindow(nullptr, {});
    window->setProjectsList(fs_.projects());
}


void UI::UIManager::openNewWindowOpenProjectSlot(const QString& projectPath) {
    if (UI::FileSystem::ProjectData data; fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        auto* newWindow = createWindow(nullptr, {data.path, data.id});
        newWindow->onOpenProjectSlot({data.path, data.id});
        newWindow->setQOpenGLPainter(new CadViewportWindow());
        for (const auto& t: data.tabs) {
            newWindow->addTabSlot(t);
        }
        newWindow->addNotification("✅ Project opened in new window: " + data.name);
    }
}


void UI::UIManager::openNewWindowCreateProjectSlot(const QString& projectPath) {
    const auto projectName = QFileInfo(projectPath).fileName();

    if (QString projectId; fs_.createProject(projectPath, projectId) == UI::FileSystem::FsResult::Ok) {
        auto* newWindow = createWindow(nullptr, {projectPath, projectId});
        newWindow->onOpenProjectSlot({projectPath, projectId});
        newWindow->setQOpenGLPainter(new CadViewportWindow());
        newWindow->addNotification("✨ Project created in new window: " + projectName);
    }
}


void UI::UIManager::openProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const {
    if (UI::FileSystem::ProjectData data; fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        window->onOpenProjectSlot({data.path, data.id});
        window->setQOpenGLPainter(new CadViewportWindow());
        for (const auto& t: data.tabs) {
            window->addTabSlot(t);
        }
        window->addNotification("✅ Project opened: " + data.name);
    }
}


void UI::UIManager::createProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const {
    const auto projectName = QFileInfo(projectPath).fileName();
    if (QString projectId; fs_.createProject(projectPath, projectId) == UI::FileSystem::FsResult::Ok) {
        window->onOpenProjectSlot({projectPath, projectId});
        window->setQOpenGLPainter(new CadViewportWindow());
        window->addNotification("✨ Project created: " + projectName);
    } else {
        window->addNotification("❌ Failed to create project: " + projectName);
    }
}


void UI::UIManager::openFileSlot(const UI::MainWindow* window, const QString& filePath) const {
    if (UI::FileSystem::FileData data; fs_.openFile(filePath, data) == UI::FileSystem::FsResult::Ok) {
        window->addTabSlot(data.tabName);
        window->addNotification("📄 File opened: " + data.tabName);
    } else {
        const auto errorMsg = "❌ Failed to open file: " + QFileInfo(filePath).fileName();
        window->addNotification(errorMsg);
    }
}


void UI::UIManager::renameTabSlot(const UI::MainWindow* window, const QString& oldName, const QString& newName) const {
    const auto projectId = window->projectID();
    if (projectId.isEmpty()) {
        window->addNotification("❌ Cannot rename tab: no active project");
        return;
    }

    if (fs_.renameTab(projectId, oldName, newName) == UI::FileSystem::FsResult::Ok) {
        window->onRenameTabSlot(oldName, newName);
        window->addNotification("✏️ Tab renamed: " + oldName + " → " + newName);
    } else {
        window->addNotification("❌ Failed to rename tab: " + oldName);
    }
}


void UI::UIManager::removeTabSlot(const UI::MainWindow* window, const QString& tabName) const {
    const auto projectId = window->projectID();
    if (projectId.isEmpty()) {
        window->addNotification("❌ Cannot remove tab: no active project");
        return;
    }

    if (fs_.removeTab(projectId, tabName) == UI::FileSystem::FsResult::Ok) {
        window->deleteTabSlot(tabName);
        window->addNotification("🗑️ Tab removed: " + tabName);
    } else {
        window->addNotification("❌ Failed to remove tab: " + tabName);
    }
}


void UI::UIManager::createFileSlot(const UI::MainWindow* window, const QString& fileName) const {
    const auto projectId = window->projectID();
    if (projectId.isEmpty()) {
        window->addNotification("❌ Cannot create file: no active project");
        return;
    }

    if (fs_.createTab(projectId, fileName) == UI::FileSystem::FsResult::Ok) {
        window->addTabSlot(fileName);
        window->addNotification("📄 File created: " + fileName);
    } else {
        window->addNotification("❌ Failed to create file: " + fileName);
    }
}


void UI::UIManager::goToStartWindowSlot(UI::MainWindow* window) const {
    window->setProjectsList(fs_.projects());
    window->onStartWindowSlot();
    window->addNotification("🏠 Returned to start page");
}


void UI::UIManager::renameProjectSlot(UI::MainWindow* window, const QString& newName, const QString& path) const {
    const auto oldName = QFileInfo(path).fileName();
    if (FileSystem::ProjectData data; fs_.renameProjectByPath(path, newName,data) == UI::FileSystem::FsResult::Ok) {
        window->setProjectsList(fs_.projects());
        window->setProjectPath(data.path);
        window->addNotification("✏️ Project renamed: " + oldName + " → " + newName);
    } else {
        window->addNotification("❌ Failed to rename project: " + oldName);
    }
}


void UI::UIManager::deleteProjectSlot(UI::MainWindow* window, const QString& path) const {
    QString projectId;
    if (fs_.getProjectIdByPath(path, projectId) != UI::FileSystem::FsResult::Ok) {
        window->addNotification("❌ Project not found: " + QFileInfo(path).fileName());
        return;
    }

    const auto projectName = QFileInfo(path).fileName();

    const bool isCurrentProject = (window->projectPath() == path || window->projectID() == projectId);

    if (fs_.removeProjectById(projectId) == UI::FileSystem::FsResult::Ok) {
        window->setProjectsList(fs_.projects());
        window->addNotification("🗑️ Project deleted: " + projectName);

        if (isCurrentProject) {
            window->onStartWindowSlot();
        }
    } else {
        window->addNotification("❌ Failed to delete project: " + projectName);
    }
}


void UI::UIManager::deleteSlot(UI::MainWindow* window) {
    mainWindows_.removeOne(window);
}


UI::MainWindow* UI::UIManager::createWindow(UI::MainWindow* parent, const UI::MainWindow::ProjectData& projectData) {
    auto* window = new UI::MainWindow(parent, projectData);
    window->show();
    mainWindows_.push_back(window);
    window->addNotification("Welcome to OurPaint! 🎨");
    initSignals(*window);

    return window;
}


void UI::UIManager::initSignals(UI::MainWindow& window) {
    QPointer<UI::MainWindow> ptrWindow(&window);

    // --- Open project in new window ---
    QObject::connect(&window, &UI::MainWindow::openNewWindowOpenProjectTriggered,
                     this, &UI::UIManager::openNewWindowOpenProjectSlot);

    // --- Create project in new window ---
    QObject::connect(&window, &UI::MainWindow::openNewWindowCreateProjectTriggered,
                     this, &UI::UIManager::openNewWindowCreateProjectSlot);

    // --- Open project in this window ---
    QObject::connect(&window, &UI::MainWindow::openProjectThisWindowTriggered,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             openProjectThisWindowSlot(ptrWindow, path);
                         }
                     });

    // --- Create project in this window ---
    QObject::connect(&window, &UI::MainWindow::createProjectThisWindowTriggered,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             createProjectThisWindowSlot(ptrWindow, path);
                         }
                     });

    // --- Open file ---
    QObject::connect(&window, &UI::MainWindow::openFileTriggered,
                     this, [this, ptrWindow](const QString& fileName) {
                         if (ptrWindow) {
                             openFileSlot(ptrWindow, fileName);
                         }
                     });

    // --- Rename tab ---
    QObject::connect(&window, &UI::MainWindow::renameTabTriggered,
                     this, [this, ptrWindow](const QString& oldName, const QString& newName) {
                         if (ptrWindow) {
                             renameTabSlot(ptrWindow, oldName, newName);
                         }
                     });

    // --- Remove tab ---
    QObject::connect(&window, &UI::MainWindow::removeTabTriggered,
                     this, [this, ptrWindow](const QString& tabName) {
                         if (ptrWindow) {
                             removeTabSlot(ptrWindow, tabName);
                         }
                     });

    // --- Create file ---
    QObject::connect(&window, &UI::MainWindow::createFileTriggered,
                     this, [this, ptrWindow](const QString& fileName) {
                         if (ptrWindow) {
                             createFileSlot(ptrWindow, fileName);
                         }
                     });

    // --- Go to start window ---
    QObject::connect(&window, &UI::MainWindow::goToStartWindowTriggered,
                     this, [this, ptrWindow] {
                         if (ptrWindow) {
                             goToStartWindowSlot(ptrWindow);
                         }
                     });

    // --- Rename project ---
    QObject::connect(&window, &UI::MainWindow::renameProjectTriggered,
                     this, [this, ptrWindow](const QString& newName, const QString& path) {
                         if (ptrWindow) {
                             renameProjectSlot(ptrWindow, newName, path);
                         }
                     });

    // --- Delete project ---
    QObject::connect(&window, &UI::MainWindow::deleteProjectTriggered,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             deleteProjectSlot(ptrWindow, path);
                         }
                     });

    // --- Cleanup ---
    QObject::connect(&window, &QObject::destroyed, this, [this](QObject* obj) {
        if (auto* w = qobject_cast<UI::MainWindow*>(obj)) {
            deleteSlot(w);
        }
    });
}
