#include "UIManager.h"

#include <QFileInfo>
#include <algorithm>

#include "CustomConsole.h"
#include "RenderEngine.h"
#include "ToolsType.h"

UIManager::UIManager() {
    setlocale(LC_ALL, "");
    auto* manager = createProjectManager();
    manager->setProjectsList(fs_.projects());
}


bool UIManager::checkedOpened(const QString& projectPath) const {
    return std::any_of(projectManagers_.begin(), projectManagers_.end(),
                       [&](const UI::ProjectManager* manager) {
                           return manager && manager->projectPath() == projectPath;
                       });
}


void UIManager::sentCommandFromConsole(UI::ProjectManager* manager, const QString& command) {
    Q_UNUSED(command);
    manager->addNotification("Console command received");
}


void UIManager::openNewWindowOpenProjectSlot(UI::ProjectManager* manager, const QString& projectPath) {
    if (checkedOpened(projectPath)) {
        manager->addNotification("Проект уже открыт");
        return;
    }

    if (UI::FileSystem::ProjectData data; fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        auto* newManager = createOpenedProjectManager({data.path, data.id}, new RenderEngine(), new CustomConsole());
      newManager->setActiveToolProjectWindow(UI::ToolsType::Size);
        for (const auto& tabName: data.tabs) {
            newManager->addTabSlot(tabName);
        }
        newManager->addNotification("✅ Project opened in new window: " + data.name);
    }
}


void UIManager::openNewWindowCreateProjectSlot(UI::ProjectManager* manager, const QString& projectPath) {
    if (checkedOpened(projectPath)) {
        manager->addNotification("Проект уже открыт");
        return;
    }

    const auto projectName = QFileInfo(projectPath).fileName();
    if (QString projectId; fs_.createProject(projectPath, projectId) == UI::FileSystem::FsResult::Ok) {
        auto* newManager =
                createOpenedProjectManager({projectPath, projectId}, new RenderEngine(), new CustomConsole());
      newManager->setActiveToolProjectWindow(UI::ToolsType::Size);
        newManager->addNotification("✨ Project created in new window: " + projectName);
    } else {
        manager->addNotification("❌ Failed to create project: " + projectName);
    }
}


void UIManager::openTabWindowSlot(UI::ProjectManager* manager, const QString& nameTab) const {
    manager->openTabWindow(nameTab, new RenderEngine(), new CustomConsole());
    manager->addNotification("✨ Tab created in new window: " + nameTab);
  manager->setActiveToolTabWindow(nameTab,UI::ToolsType::Size);
}


void UIManager::openProjectThisWindowSlot(UI::ProjectManager* manager, const QString& projectPath) const {
    if (checkedOpened(projectPath)) {
        manager->addNotification("Проект уже открыт: " + projectPath);
        return;
    }

    if (UI::FileSystem::ProjectData data; fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        manager->openProjectWindow({data.path, data.id},new RenderEngine(),new CustomConsole());
        for (const auto& tabName: data.tabs) {
            manager->addTabSlot(tabName);
        }
        manager->addNotification("✅ Project opened: " + data.name);
      manager->setActiveToolProjectWindow(UI::ToolsType::Size);
    }
}


void UIManager::createProjectThisWindowSlot(UI::ProjectManager* manager, const QString& projectPath) const {
    const auto projectName = QFileInfo(projectPath).fileName();
    if (QString projectId; fs_.createProject(projectPath, projectId) == UI::FileSystem::FsResult::Ok) {
        manager->openProjectWindow({projectPath, projectId}, new RenderEngine(), new CustomConsole());
        manager->addNotification("✨ Project created: " + projectName);
      manager->setActiveToolProjectWindow(UI::ToolsType::Size);
    } else {
        manager->addNotification("❌ Failed to create project: " + projectName);
    }
}


void UIManager::openFileSlot(UI::ProjectManager* manager, const QString& filePath) const {
    if (UI::FileSystem::FileData data; fs_.openFile(filePath, data) == UI::FileSystem::FsResult::Ok) {
        manager->addTabSlot(data.tabName);
        manager->addNotification("📄 File opened: " + data.tabName);
    } else {
        manager->addNotification("❌ Failed to open file: " + QFileInfo(filePath).fileName());
    }
}


void UIManager::renameTabSlot(UI::ProjectManager* manager, const QString& oldName, const QString& newName) const {
    const auto projectId = manager->projectID();
    if (projectId.isEmpty()) {
        manager->addNotification("❌ Cannot rename tab: no active project");
        return;
    }

    if (fs_.renameTab(projectId, oldName, newName) == UI::FileSystem::FsResult::Ok) {
        manager->renameTabSlot(oldName, newName);
        manager->addNotification("✏️ Tab renamed: " + oldName + " → " + newName);
    } else {
        manager->addNotification("❌ Failed to rename tab: " + oldName);
    }
}


void UIManager::removeTabSlot(UI::ProjectManager* manager, const QString& tabName) const {
    const auto projectId = manager->projectID();
    if (projectId.isEmpty()) {
        manager->addNotification("❌ Cannot remove tab: no active project");
        return;
    }

    if (fs_.removeTab(projectId, tabName) == UI::FileSystem::FsResult::Ok) {
        manager->deleteTabSlot(tabName);
        manager->addNotification("🗑️ Tab removed: " + tabName);
    } else {
        manager->addNotification("❌ Failed to remove tab: " + tabName);
    }
}


void UIManager::createFileSlot(UI::ProjectManager* manager, const QString& fileName) const {
    const auto projectId = manager->projectID();
    if (projectId.isEmpty()) {
        manager->addNotification("❌ Cannot create file: no active project");
        return;
    }

    if (fs_.createTab(projectId, fileName) == UI::FileSystem::FsResult::Ok) {
        manager->addTabSlot(fileName);
        manager->addNotification("📄 File created: " + fileName);
    } else {
        manager->addNotification("❌ Failed to create file: " + fileName);
    }
}


void UIManager::goToStartWindowSlot(UI::ProjectManager* manager) const {
    manager->setProjectsList(fs_.projects());
    manager->openStartWindow();
    manager->addNotification("🏠 Returned to start page");
}


void UIManager::renameProjectSlot(UI::ProjectManager* manager, const QString& newName, const QString& path) const {
    const auto oldName = QFileInfo(path).fileName();
    if (UI::FileSystem::ProjectData data; fs_.renameProjectByPath(path, newName, data) == UI::FileSystem::FsResult::Ok) {
        manager->setProjectsList(fs_.projects());
        manager->setProjectPath(data.path);
        manager->addNotification("✏️ Project renamed: " + oldName + " → " + newName);
    } else {
        manager->addNotification("❌ Failed to rename project: " + oldName);
    }
}


void UIManager::deleteProjectSlot(UI::ProjectManager* manager, const QString& path) const {
    QString projectId;
    if (fs_.getProjectIdByPath(path, projectId) != UI::FileSystem::FsResult::Ok) {
        manager->addNotification("❌ Project not found: " + QFileInfo(path).fileName());
        return;
    }

    const auto projectName = QFileInfo(path).fileName();
    const bool isCurrentProject = (manager->projectPath() == path || manager->projectID() == projectId);

    if (fs_.removeProjectById(projectId) == UI::FileSystem::FsResult::Ok) {
        manager->setProjectsList(fs_.projects());
        manager->addNotification("🗑️ Project deleted: " + projectName);

        if (isCurrentProject) {
            manager->openStartWindow();
        }
    } else {
        manager->addNotification("❌ Failed to delete project: " + projectName);
    }
}


void UIManager::deleteSlot(UI::ProjectManager* manager) {
    projectManagers_.removeOne(manager);
}


void UIManager::primitiveSlot(UI::ProjectManager* manager, UI::PrimitiveType& type) {
    Q_UNUSED(type);
    manager->addNotification("Primitive slot selected");
}


void UIManager::constraintSlot(UI::ProjectManager* manager, UI::ConstraintType& type) {
    Q_UNUSED(type);
    manager->addNotification("Constraint slot selected");
}


void UIManager::toolsSlot(UI::ProjectManager* manager, UI::ToolsType& type) {
    Q_UNUSED(type);
    manager->addNotification("Tools slot");
}
void UIManager::settingsSlot(UI::ProjectManager *manager, double f, double s, double t) {
  qDebug()<<f<<s<<t;
}

UI::ProjectManager* UIManager::createOpenedProjectManager(const UI::ProjectManager::ProjectData& projectData,
                                                              QWindow* windowRender,
                                                              QLineEdit* consoleEngine) {
    auto* manager = new UI::ProjectManager(projectData, windowRender, consoleEngine);
    projectManagers_.push_back(manager);
    manager->setDefaultProjectsPath(fs_.defaultProjectsPath());
    manager->setProjectsList(fs_.projects());
    manager->addNotification("Welcome to OurPaint! 🎨");
    initSignals(*manager);
    return manager;
}


UI::ProjectManager* UIManager::createProjectManager(const UI::ProjectManager::ProjectData& projectData) {
    auto* manager = new UI::ProjectManager(projectData);
    projectManagers_.push_back(manager);
    manager->setDefaultProjectsPath(fs_.defaultProjectsPath());
    manager->setProjectsList(fs_.projects());
    manager->addNotification("Welcome to OurPaint! 🎨");
    initSignals(*manager);
    return manager;
}


void UIManager::initSignals(UI::ProjectManager& manager) {
    connect(&manager, &UI::ProjectManager::sentCommandTriggered, this,
            [this, &manager](const QString& command) {
                sentCommandFromConsole(&manager, command);
            });

    connect(&manager, &UI::ProjectManager::openNewWindowOpenProjectTriggered, this,
            [this, &manager](const QString& path) {
                openNewWindowOpenProjectSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::openNewWindowCreateProjectTriggered, this,
            [this, &manager](const QString& path) {
                openNewWindowCreateProjectSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::openProjectThisWindowTriggered, this,
            [this, &manager](const QString& path) {
                openProjectThisWindowSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::openNewWindowTabTriggered, this,
            [this, &manager](const QString& name) {
                openTabWindowSlot(&manager, name);
            });

    connect(&manager, &UI::ProjectManager::createProjectThisWindowTriggered, this,
            [this, &manager](const QString& path) {
                createProjectThisWindowSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::openFileTriggered, this,
            [this, &manager](const QString& fileName) {
                openFileSlot(&manager, fileName);
            });

    connect(&manager, &UI::ProjectManager::renameTabTriggered, this,
            [this, &manager](const QString& oldName, const QString& newName) {
                renameTabSlot(&manager, oldName, newName);
            });

    connect(&manager, &UI::ProjectManager::removeTabTriggered, this,
            [this, &manager](const QString& tabName) {
                removeTabSlot(&manager, tabName);
            });

    connect(&manager, &UI::ProjectManager::createFileTriggered, this,
            [this, &manager](const QString& fileName) {
                createFileSlot(&manager, fileName);
            });

    connect(&manager, &UI::ProjectManager::goToStartWindowTriggered, this,
            [this, &manager]() {
                goToStartWindowSlot(&manager);
            });

    connect(&manager, &UI::ProjectManager::renameProjectTriggered, this,
            [this, &manager](const QString& newName, const QString& path) {
                renameProjectSlot(&manager, newName, path);
            });

    connect(&manager, &UI::ProjectManager::deleteProjectTriggered, this,
            [this, &manager](const QString& path) {
                deleteProjectSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::primitiveTriggered, this,
            [this, &manager](UI::PrimitiveType& type) {
                primitiveSlot(&manager, type);
            });

    connect(&manager, &UI::ProjectManager::constraintTriggered, this,
            [this, &manager](UI::ConstraintType& type) {
                constraintSlot(&manager, type);
            });

    connect(&manager, &UI::ProjectManager::toolsTriggered, this,
            [this, &manager](UI::ToolsType& type) {
                toolsSlot(&manager, type);
            });

  connect(&manager, &UI::ProjectManager::applySettings, this,
        [this, &manager](double f,double s,double t) {
            settingsSlot(&manager, f,s,t);
        });

    connect(&manager, &QObject::destroyed, this,
            [this, &manager] {
                deleteSlot(&manager);
            });
}
