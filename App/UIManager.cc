#include "UIManager.h"

#include <clocale>
#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QTranslator>
#include <QLineEdit>

#include "CustomConsole.h"
#include "RenderEngine.h"
#include "ToolsType.h"
#include "Utils.h"

UIManager::UIManager() : settingsSystem_(fs_.settingsPath()) {
    setlocale(LC_ALL, "");

    currentLanguage_ = settingsSystem_.language();
    applyLanguage(currentLanguage_);

    auto ctx = createEditorContext();
    auto m = createBaseManager(ctx);
    m->showStartWindow();
}

UIManager::~UIManager() {
    for (auto* pm : projectManagers_) {
        if (pm) {
            disconnect(pm, nullptr, this, nullptr);
        }
    }
}

UI::EditorContext UIManager::createEditorContext() const {
    UI::EditorContext ctx;
    ctx.fileSystem = const_cast<UI::FileSystem*>(&fs_);
    ctx.settings = const_cast<UI::SettingsSystem*>(&settingsSystem_);
    ctx.renderEngine = new RenderEngine();
    ctx.consoleEngine = new CustomConsole();
    return ctx;
}

UI::EditorContext UIManager::createLightEditorContext() const {
    return createEditorContext();
}

UI::ProjectManager* UIManager::createBaseManager(const UI::EditorContext& context) {
    auto *manager = new UI::ProjectManager(context, this);
    projectManagers_.push_back(manager);
    initBaseSignals(*manager);
    return manager;
}

UI::ProjectManager* UIManager::createLightManager(const UI::EditorContext& context) {
    auto *manager = new UI::ProjectManager(context, this);
    projectManagers_.push_back(manager);
    initLightManager(*manager);
    return manager;
}

void UIManager::deleteSlot(UI::ProjectManager *manager) {
    projectManagers_.removeOne(manager);
}

void UIManager::openFilesFromCommandLine(const QStringList& paths) {
    if (paths.isEmpty()) return;

    UI::ProjectManager* target = nullptr;
    for (auto* pm : projectManagers_) {
        if (!pm->isLightEditor()) {
            target = pm;
            break;
        }
    }

    if (!target) return;

    for (const QString& path : paths) {
        if (path.trimmed().isEmpty()) continue;
        openFileByPath(path, target, false);
    }
}

void UIManager::openFile(qint64 fileId, UI::ProjectManager *currentManager, bool newWindow) {
    UI::FileSystem::ProjectData data;
    if (fs_.getProject(fileId, data) != UI::FileSystem::FsResult::Ok) {
        if (currentManager) currentManager->addNotification("❌ File not found: id " + QString::number(fileId));
        return;
    }

    UI::FileData fd{data.id, data.name};

    if (newWindow || !currentManager) {
        auto *newManager = createBaseManager(createEditorContext());
        newManager->showEditorWindow(fd);
        newManager->addNotification("📄 File opened in new window: " + data.name);
    } else {
        currentManager->addTabSlot(fd);
        currentManager->addNotification("📄 File opened: " + data.name);
    }
}

void UIManager::openFileByPath(const QString& path, UI::ProjectManager *currentManager, bool newWindow) {
    if (path.trimmed().isEmpty()) return;

    qint64 fileId = -1;
    const auto res = fs_.registerProject(path, fileId);
    if (res != UI::FileSystem::FsResult::Ok && res != UI::FileSystem::FsResult::AlreadyExists) {
        if (currentManager) currentManager->addNotification("❌ Failed to open file: " + QFileInfo(path).fileName());
        return;
    }

    openFile(fileId, currentManager, newWindow);
}

void UIManager::createFile(const QString& fileName, UI::ProjectManager *currentManager, bool newWindow) {
    qint64 fileId = -1;
    const auto res = fs_.createProject(fileName, fileId);

    switch (res) {
        case UI::FileSystem::FsResult::Ok:
            break;
        case UI::FileSystem::FsResult::AlreadyExists:
            if (currentManager) currentManager->addNotification("❌ File already exists: " + fileName);
            return;
        case UI::FileSystem::FsResult::InvalidArgument:
            if (currentManager) currentManager->addNotification("❌ Invalid file name: " + fileName);
            return;
        default:
            if (currentManager) currentManager->addNotification("❌ Failed to create file: " + fileName);
            return;
    }

    UI::FileSystem::ProjectData data;
    if (fs_.getProject(fileId, data) != UI::FileSystem::FsResult::Ok) return;

    UI::FileData fd{data.id, data.name};

    if (newWindow || !currentManager) {
        auto *newManager = createBaseManager(createEditorContext());
        newManager->showEditorWindow(fd);
        newManager->addNotification("✨ File created in new window: " + data.name);
    } else {
        currentManager->addTabSlot(fd);
        currentManager->addNotification("✨ File created: " + data.name);
    }
}

void UIManager::renameFile(UI::FileData& data) {
    if (data.name.isEmpty()) return;

    if (fs_.renameProject(data.id, data.name) == UI::FileSystem::FsResult::Ok) {
        for (auto *m: projectManagers_) {
            m->renameTabSlot(data);
        }
    }
}

void UIManager::deleteFile(UI::ProjectManager *manager, qint64 fileId) {
    UI::FileSystem::ProjectData data;
    const bool hasData = fs_.getProject(fileId, data) == UI::FileSystem::FsResult::Ok;
    const QString name = hasData ? data.name : QString::number(fileId);

    if (fs_.removeProject(fileId) != UI::FileSystem::FsResult::Ok) {
        if (manager) manager->addNotification("❌ Failed to remove file: " + name);
        return;
    }

    for (auto *m : projectManagers_) {
        if (m) m->deleteTabSlot(fileId);
    }
    if (manager) manager->addNotification("🗑️ File removed: " + name);
}

void UIManager::closeTab(UI::ProjectManager *currentManager, qint64 fileId) {
    if (currentManager) {
        currentManager->closeTabSlot(fileId);
    }
}

void UIManager::closeLightEditorWindow(const qint64 fileId, UI::ProjectManager *currentManager) {
    Q_UNUSED(fileId);
    Q_UNUSED(currentManager);
}

void UIManager::openLightEditor(UI::ProjectManager *currentManager, UI::FileData& data) {
    if (!currentManager) return;

    auto ctx = createLightEditorContext();
    auto* lightManager = createLightManager(ctx);

    lightManager->showLightEditorWindow(data);
    currentManager->closeTabSlot(data.id);

    lightManager->addNotification("Opened light editor: " + data.name);
}

void UIManager::returnTabFromLight(UI::ProjectManager *manager, qint64 fileId) {
    UI::FileSystem::ProjectData pd;
    if (fs_.getProject(fileId, pd) != UI::FileSystem::FsResult::Ok) return;

    UI::FileData fd{pd.id, pd.name};

    UI::ProjectManager* targetManager = nullptr;
    for (auto* pm : projectManagers_) {
        if (!pm->isLightEditor() && pm != manager) {
            targetManager = pm;
            break;
        }
    }

    if (!targetManager) {
        targetManager = createBaseManager(createEditorContext());
        targetManager->showEditorWindow(fd);
    }else {
        targetManager->addTabSlot(fd);
    }

    targetManager->addNotification("Returned tab from light editor: " + fd.name);

    if (manager) {
        manager->deleteLater();
    }
}

void UIManager::handleTabMove(qint64 fileId, quintptr sourceWindowPtr, quintptr targetWindowPtr) {
    UI::ProjectManager* sourceManager = nullptr;
    UI::ProjectManager* targetManager = nullptr;

    for (auto* pm : projectManagers_) {
        if (!pm) continue;

        QWidget* win = pm->windowWidget();
        if (!win) continue;

        const quintptr windowPtr = reinterpret_cast<quintptr>(win);
        if (windowPtr == sourceWindowPtr) sourceManager = pm;
        if (windowPtr == targetWindowPtr) targetManager = pm;
    }

    if (!sourceManager || !targetManager || sourceManager == targetManager) return;

    UI::FileSystem::ProjectData pd;
    if (fs_.getProject(fileId, pd) != UI::FileSystem::FsResult::Ok) return;

    UI::FileData fd{pd.id, pd.name};
    sourceManager->closeTabSlot(fileId);
    targetManager->addTabSlot(fd);
}

void UIManager::setActiveTab(UI::ProjectManager *manager, qint64 fileId) const {
    if (manager) manager->setActiveTab(fileId);
}

void UIManager::sentCommandFromConsole(UI::ProjectManager *manager, qint64 fileId, const QString& command) const {
    if (manager) manager->addNotification(fileId, "Console command received: " + command);
}

void UIManager::primitiveSlot(UI::ProjectManager *manager, UI::PrimitiveType& type, qint64 fileId) {
    Q_UNUSED(type);
    if (manager) manager->addNotification(fileId, "Primitive selected for file " + QString::number(fileId));
}

void UIManager::constraintSlot(UI::ProjectManager *manager, UI::ConstraintType& type, qint64 fileId) {
    Q_UNUSED(type);
    if (manager) manager->addNotification(fileId, "Constraint selected for file " + QString::number(fileId));
}

void UIManager::toolsSlot(UI::ProjectManager *manager, UI::ToolsType& type, qint64 fileId) {
    Q_UNUSED(type);
    if (manager) manager->addNotification(fileId, "Tool selected for file " + QString::number(fileId));
}

void UIManager::scriptSlot(UI::ProjectManager *manager, const QString& path) {
    if (manager) manager->addNotification("Script: " + path);
}

void UIManager::settingsChangedSlot(UI::ProjectManager *) {
    const auto d = settingsSystem_.loadChanges();
    if (d.isEmpty()) return;

    if (!d.sketch.isEmpty()) {
        qDebug() << "[settings] sketch changed:"
                << (d.sketch.gridEnabled ? *d.sketch.gridEnabled : -1)
                << (d.sketch.snapEnabled ? *d.sketch.snapEnabled : -1)
                << (d.sketch.gridStep ? *d.sketch.gridStep : -1)
                << (d.sketch.snapStep ? *d.sketch.snapStep : -1);
    }

    if (d.language) applyLanguage(*d.language);
}

void UIManager::applyLanguage(UI::SettingsSystem::Language lang) {
    if (translator_) {
        qApp->removeTranslator(translator_);
        delete translator_;
        translator_ = nullptr;
    }

    currentLanguage_ = lang;

    if (lang == UI::SettingsSystem::Language::English) {
        return;
    }

    const QString path = (lang == UI::SettingsSystem::Language::Russian)
                             ? QStringLiteral(":/translations/app_ru.qm")
                             : QStringLiteral(":/translations/app_by.qm");

    auto *tr = new QTranslator(this);
    if (tr->load(path)) {
        qApp->installTranslator(tr);
        translator_ = tr;
    } else {
        delete tr;
    }
}

void UIManager::initBaseSignals(UI::ProjectManager& manager) {
    connect(&manager, &UI::ProjectManager::openFileByIDTriggered, this,
            [this, &manager](qint64 fileId, bool newWindow) { openFile(fileId, &manager, newWindow); });

    connect(&manager, &UI::ProjectManager::openFileTriggered, this,
            [this, &manager](const QString& path, bool newWindow) { openFileByPath(path, &manager, newWindow); });

    connect(&manager, &UI::ProjectManager::createFileTriggered, this,
            [this, &manager](const QString& fileName, bool newWindow) { createFile(fileName, &manager, newWindow); });

    connect(&manager, &UI::ProjectManager::renameFileTriggered, this,
            [this](UI::FileData& data) { renameFile(data); });

    connect(&manager, &UI::ProjectManager::removeFileTriggered, this,
            [this, &manager](qint64 fileId) { deleteFile(&manager, fileId); });

    connect(&manager, &UI::ProjectManager::closeFileTriggered, this,
            [this, &manager](const qint64 fileId) { closeTab(&manager, fileId); });

    connect(&manager, &UI::ProjectManager::setActiveFileTriggered, this,
            [this, &manager](qint64 fileId) { setActiveTab(&manager, fileId); });

    connect(&manager, &UI::ProjectManager::lightEditorRequested, this,
            [this, &manager](qint64 fileId) {
                UI::FileSystem::ProjectData pd;
                if (fs_.getProject(fileId, pd) != UI::FileSystem::FsResult::Ok) return;
                UI::FileData fd{pd.id, pd.name};
                openLightEditor(&manager, fd);
            });
    connect(&manager, &UI::ProjectManager::tabMovedToAnotherWindow, this,
            [this](qint64 fileId, quintptr sourceWindowPtr, quintptr targetWindowPtr) {
                handleTabMove(fileId, sourceWindowPtr, targetWindowPtr);
            });

    connect(&manager, &UI::ProjectManager::saveTriggered, this,
            []() { std::cout << "Saving full project..." << std::endl; });

    connect(&manager, &UI::ProjectManager::closeEditorTriggered, this,
           []() { std::cout << "Saving full project..." << std::endl; });

    connect(&manager, &UI::ProjectManager::sentCommandTriggered, this,
            [this, &manager](qint64 fileId, const QString& command) { sentCommandFromConsole(&manager, fileId, command); });

    connect(&manager, &UI::ProjectManager::primitiveTriggered, this,
            [this, &manager](qint64 fileId, UI::PrimitiveType type) { primitiveSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::constraintTriggered, this,
            [this, &manager](qint64 fileId, UI::ConstraintType type) { constraintSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::toolsTriggered, this,
            [this, &manager](qint64 fileId, UI::ToolsType type) { toolsSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::scriptTriggered, this,
            [this, &manager](const QString& path) { scriptSlot(&manager, path); });

    connect(&manager, &UI::ProjectManager::settingsChanged, this,
            [this, &manager]() { settingsChangedSlot(&manager); });

    connect(&manager, &QObject::destroyed, this,
            [this, &manager] { deleteSlot(&manager); });
}

void UIManager::initLightManager(UI::ProjectManager& manager) {
    connect(&manager, &UI::ProjectManager::returnTabToEditorRequested, this,
            [this, &manager](qint64 fileId) { returnTabFromLight(&manager, fileId); });

    connect(&manager, &UI::ProjectManager::closeLightEditorWindowTriggered, this,
            [this, &manager](qint64 fileId) { closeLightEditorWindow(fileId, &manager); });

    connect(&manager, &UI::ProjectManager::sentCommandTriggered, this,
            [this, &manager](qint64 fileId, const QString& command) { sentCommandFromConsole(&manager, fileId, command); });

    connect(&manager, &UI::ProjectManager::primitiveTriggered, this,
            [this, &manager](qint64 fileId, UI::PrimitiveType type) { primitiveSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::constraintTriggered, this,
            [this, &manager](qint64 fileId, UI::ConstraintType type) { constraintSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::toolsTriggered, this,
            [this, &manager](qint64 fileId, UI::ToolsType type) { toolsSlot(&manager, type, fileId); });

    connect(&manager, &UI::ProjectManager::scriptTriggered, this,
            [this, &manager](const QString& path) { scriptSlot(&manager, path); });

    connect(&manager, &UI::ProjectManager::settingsChanged, this,
            [this, &manager]() { settingsChangedSlot(&manager); });

    connect(&manager, &UI::ProjectManager::saveTriggered, this,
    [this, &manager]() { });

    connect(&manager, &QObject::destroyed, this,
            [this, &manager] { deleteSlot(&manager); });
}