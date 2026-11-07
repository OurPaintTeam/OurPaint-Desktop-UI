#include "UIManager.h"

#include <clocale>
#include <QDebug>
#include <QFileInfo>

#include "CustomConsole.h"
#include "RenderEngine.h"
#include "ToolsType.h"
#include "Utils.h"

UIManager::UIManager() {
    setlocale(LC_ALL, "");
    auto* manager = createProjectManager();
    refreshFiles(manager);
}


qint64 UIManager::defaultLocationId() const {
    const auto locations = fs_.locations();
    for (const auto& loc : locations) {
        if (loc.isDefault) return loc.id;
    }
    return locations.isEmpty() ? -1 : locations.first().id;
}


void UIManager::refreshFiles(UI::ProjectManager* manager) const {
    if (!manager) return;

    QVector<UI::FileData> files;
    for (const auto& f : fs_.files()) {
        UI::FileData fd;
        fd.id = f.id;
        fd.name = f.name;
        fd.path = f.path;
        files.append(fd);
    }
    manager->setFilesList(files);
}


UI::ProjectManager* UIManager::createProjectManager() {
    auto* manager = new UI::ProjectManager();
    projectManagers_.push_back(manager);
    refreshFiles(manager);
    initSignals(*manager);
    return manager;
}


UI::ProjectManager* UIManager::createEditorProjectManager(QWindow* windowRender,
                                                          QLineEdit* consoleEngine) {
    auto* manager = new UI::ProjectManager(windowRender, consoleEngine);
    projectManagers_.push_back(manager);
    refreshFiles(manager);
    initSignals(*manager);
    return manager;
}


void UIManager::deleteSlot(UI::ProjectManager* manager) {
    projectManagers_.removeOne(manager);
}


void UIManager::openFileInCurrentWindow(UI::ProjectManager* manager, qint64 fileId) const {
    UI::FileSystem::FileData data;
    if (fs_.openFile(fileId, data) != UI::FileSystem::FsResult::Ok) {
        manager->addNotification("❌ File not found: id " + QString::number(fileId));
        return;
    }
    manager->addTabSlot(data.name, fileId);
    manager->addNotification("📄 File opened: " + data.name);
}


void UIManager::openFileByPathInCurrentWindow(UI::ProjectManager* manager, const QString& path) const {
    if (path.trimmed().isEmpty()) return;

    qint64 fileId = -1;
    if (fs_.getFileIdByPath(path, fileId) != UI::FileSystem::FsResult::Ok) {
        if (fs_.registerFile(path, fileId) != UI::FileSystem::FsResult::Ok) {
            manager->addNotification("❌ Failed to open file: " + QFileInfo(path).fileName());
            return;
        }
    }
    openFileInCurrentWindow(manager, fileId);
}


void UIManager::openFileByPathInNewWindow(const QString& path) {
    if (path.trimmed().isEmpty()) return;

    qint64 fileId = -1;
    if (fs_.getFileIdByPath(path, fileId) != UI::FileSystem::FsResult::Ok) {
        if (fs_.registerFile(path, fileId) != UI::FileSystem::FsResult::Ok) {
            return;
        }
    }

    UI::FileSystem::FileData data;
    if (fs_.openFile(fileId, data) != UI::FileSystem::FsResult::Ok) return;

    auto* manager = createEditorProjectManager(new RenderEngine(), new CustomConsole());
    manager->setActiveToolEditorWindow(UI::ToolsType::Size);
    manager->addTabSlot(data.name, fileId);
    manager->addNotification("📄 File opened in new window: " + data.name);
}


void UIManager::createFileInCurrentWindow(UI::ProjectManager* manager, const QString& fileName) const {
    const qint64 locationId = defaultLocationId();
    if (locationId == -1) {
        manager->addNotification("❌ Cannot create file: no registered location");
        return;
    }

    qint64 fileId = -1;
    if (fs_.createFile(locationId, fileName, fileId) != UI::FileSystem::FsResult::Ok) {
        manager->addNotification("❌ Failed to create file: " + fileName);
        return;
    }
    openFileInCurrentWindow(manager, fileId);
}


void UIManager::createFileInNewWindow(const QString& fileName) {
    const qint64 locationId = defaultLocationId();
    if (locationId == -1) return;

    qint64 fileId = -1;
    if (fs_.createFile(locationId, fileName, fileId) != UI::FileSystem::FsResult::Ok) return;

    UI::FileSystem::FileData data;
    if (fs_.openFile(fileId, data) != UI::FileSystem::FsResult::Ok) return;

    auto* manager = createEditorProjectManager(new RenderEngine(), new CustomConsole());
    manager->setActiveToolEditorWindow(UI::ToolsType::Size);
    manager->addTabSlot(data.name, fileId);
    manager->addNotification("✨ File created in new window: " + data.name);
}


void UIManager::renameFile(qint64 fileId, const QString& newName) const {
    if (newName.isEmpty()) return;

    UI::FileSystem::FileData old;
    const bool hasOld = fs_.openFile(fileId, old) == UI::FileSystem::FsResult::Ok;
    const QString oldName = hasOld ? old.name : QString::number(fileId);

    if (fs_.renameFile(fileId, newName) != UI::FileSystem::FsResult::Ok) {
        return;
    }

    for (auto* manager : projectManagers_) {
        if (manager) manager->renameTabSlot(fileId, newName);
    }
}


void UIManager::deleteFile(UI::ProjectManager* manager, qint64 fileId) const {
    UI::FileSystem::FileData data;
    const bool hasData = fs_.openFile(fileId, data) == UI::FileSystem::FsResult::Ok;
    const QString name = hasData ? data.name : QString::number(fileId);

    if (fs_.removeFile(fileId) != UI::FileSystem::FsResult::Ok) {
        manager->addNotification("❌ Failed to remove file: " + name);
        return;
    }

    for (auto* m : projectManagers_) {
        if (m) m->deleteTabSlot(fileId);
    }
    refreshFiles(manager);
    manager->addNotification("🗑️ File removed: " + name);
}


void UIManager::openLightEditor(UI::ProjectManager* manager, const QString& fileName, qint64 fileId) {
    if (!manager) return;
    manager->openLightEditorWindow(fileName, fileId, new RenderEngine(), new CustomConsole());
    manager->setActiveToolLightEditorWindow(fileId, UI::ToolsType::Size);
}


void UIManager::returnTabFromLight(UI::ProjectManager* manager, qint64 fileId) const {
    if (!manager) return;
    UI::FileSystem::FileData data;
    if (fs_.openFile(fileId, data) != UI::FileSystem::FsResult::Ok) return;
    manager->addTabSlot(data.name, fileId);
}


void UIManager::setActiveTab(UI::ProjectManager* manager, qint64 fileId) const {
    if (manager) manager->setActiveTab(fileId);
}


void UIManager::goToStartWindow(UI::ProjectManager* manager) const {
    if (!manager) return;
    refreshFiles(manager);
    manager->openStartWindow();
}


void UIManager::sentCommandFromConsole(UI::ProjectManager* manager, qint64 fileId,
                                       const QString& command) const {
    manager->addNotification(fileId, "Console command received: " + command);
}


void UIManager::primitiveSlot(UI::ProjectManager* manager, UI::PrimitiveType& type, qint64 fileId) {
    Q_UNUSED(type);
    manager->addNotification(fileId, "Primitive selected for file " + QString::number(fileId));
}


void UIManager::constraintSlot(UI::ProjectManager* manager, UI::ConstraintType& type, qint64 fileId) {
    Q_UNUSED(type);
    manager->addNotification(fileId, "Constraint selected for file " + QString::number(fileId));
}


void UIManager::toolsSlot(UI::ProjectManager* manager, UI::ToolsType& type, qint64 fileId) {
    Q_UNUSED(type);
    manager->addNotification(fileId, "Tool selected for file " + QString::number(fileId));
}


void UIManager::scriptSlot(UI::ProjectManager* manager, const QString& path) {
    manager->addNotification("Script: " + path);
}


void UIManager::settingsSlot(UI::ProjectManager* manager, double f, double s, double t) {
    Q_UNUSED(manager);
    qDebug() << f << s << t;
}


void UIManager::initSignals(UI::ProjectManager& manager) {
    connect(&manager, &UI::ProjectManager::openFileThisWindowTriggered, this,
            [this, &manager](const QString& path) {
                openFileByPathInCurrentWindow(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::createFileThisWindowTriggered, this,
            [this, &manager](const QString& fileName) {
                createFileInCurrentWindow(&manager, fileName);
            });

    connect(&manager, &UI::ProjectManager::renameFileTriggered, this,
            [this](qint64 fileId, const QString& newName) {
                renameFile(fileId, newName);
            });

    connect(&manager, &UI::ProjectManager::deleteFileTriggered, this,
            [this, &manager](qint64 fileId) {
                deleteFile(&manager, fileId);
            });

    connect(&manager, &UI::ProjectManager::openNewWindowOpenFileTriggered, this,
            [this](const QString& path) {
                openFileByPathInNewWindow(path);
            });

    connect(&manager, &UI::ProjectManager::openNewWindowCreateFileTriggered, this,
            [this](const QString& fileName) {
                createFileInNewWindow(fileName);
            });

    connect(&manager, &UI::ProjectManager::openFileTriggered, this,
            [this, &manager](const QString& path) {
                openFileByPathInCurrentWindow(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::createFileTriggered, this,
            [this, &manager](const QString& fileName) {
                createFileInCurrentWindow(&manager, fileName);
            });

    connect(&manager, &UI::ProjectManager::renameTabTriggered, this,
            [this](qint64 fileId, const QString& newName) {
                renameFile(fileId, newName);
            });

    connect(&manager, &UI::ProjectManager::removeTabTriggered, this,
            [this, &manager](qint64 fileId) {
                deleteFile(&manager, fileId);
            });

    connect(&manager, &UI::ProjectManager::setActiveTabTriggered, this,
            [this, &manager](qint64 fileId) {
                setActiveTab(&manager, fileId);
            });

    connect(&manager, &UI::ProjectManager::goToStartWindowTriggered, this,
            [this, &manager]() {
                goToStartWindow(&manager);
            });

    connect(&manager, &UI::ProjectManager::openLightEditorTriggered, this,
            [this, &manager](qint64 fileId) {
                UI::FileData data;
               UI::FileSystem::FsResult f =  fs_.getFileData(fileId,data);
                openLightEditor(&manager, data.name, fileId);
            });

    connect(&manager, &UI::ProjectManager::returnLightEditorWindowTriggered, this,
            [this, &manager](qint64 fileId) {
                returnTabFromLight(&manager, fileId);
            });

    connect(&manager, &UI::ProjectManager::sentCommandTriggered, this,
            [this, &manager](qint64 fileId, const QString& command) {
                sentCommandFromConsole(&manager, fileId, command);
            });

    connect(&manager, &UI::ProjectManager::primitiveTriggered, this,
            [this, &manager](qint64 fileId, UI::PrimitiveType type) {
                primitiveSlot(&manager, type, fileId);
            });

    connect(&manager, &UI::ProjectManager::constraintTriggered, this,
            [this, &manager](qint64 fileId, UI::ConstraintType type) {
                constraintSlot(&manager, type, fileId);
            });

    connect(&manager, &UI::ProjectManager::toolsTriggered, this,
            [this, &manager](qint64 fileId, UI::ToolsType type) {
                toolsSlot(&manager, type, fileId);
            });

    connect(&manager, &UI::ProjectManager::scriptTriggered, this,
            [this, &manager](const QString& path) {
                scriptSlot(&manager, path);
            });

    connect(&manager, &UI::ProjectManager::applySettings, this,
            [this, &manager](double f, double s, double t) {
                settingsSlot(&manager, f, s, t);
            });

    connect(&manager, &QObject::destroyed, this,
            [this, &manager] {
                deleteSlot(&manager);
            });
}