#include "UIManager.h"

#include <clocale>
#include <QDebug>
#include <QFileInfo>
#include <QTranslator>

#include "CustomConsole.h"
#include "RenderEngine.h"
#include "ToolsType.h"
#include "Utils.h"

UIManager::UIManager() : settingsSystem_(fs_.settingsPath()) {
    setlocale(LC_ALL, "");

    currentLanguage_ = settingsSystem_.language();
    applyLanguage(currentLanguage_);
    // TODO all settings

    auto *manager = createStartWindow();
    manager->createStartWindow(collectProjectData(), fs_.projectsPath());
}


void UIManager::settingsChangedSlot(UI::ProjectManager*) {
    const auto d = settingsSystem_.loadChanges();
    if (d.isEmpty()) return;

    if (!d.sketch.isEmpty()) {

        qDebug() << "[settings] sketch changed:"
                 << (d.sketch.gridEnabled ? *d.sketch.gridEnabled : -1)
                 << (d.sketch.snapEnabled ? *d.sketch.snapEnabled : -1)
                 << (d.sketch.gridStep    ? *d.sketch.gridStep    : -1)
                 << (d.sketch.snapStep    ? *d.sketch.snapStep    : -1);
    }

    if (d.language)
        applyLanguage(*d.language);
}



UI::ProjectManager* UIManager::createStartWindow() {
    auto* manager = new UI::ProjectManager();
    projectManagers_.push_back(manager);
    initSignals(*manager);
    return manager;
}


UI::ProjectManager* UIManager::createEditorProjectManager(QWindow* windowRender,
                                                          QLineEdit* consoleEngine) {
    auto* manager = new UI::ProjectManager();
    manager->createEditorWindow(windowRender, consoleEngine,&settingsSystem_, fs_.projectsPath(), fs_.scriptsPath());
    projectManagers_.push_back(manager);
    initSignals(*manager);
    return manager;
}


void UIManager::deleteSlot(UI::ProjectManager* manager) {
    projectManagers_.removeOne(manager);
}


void UIManager::refreshFiles(UI::ProjectManager* manager)  {
    if (!manager) return;
    manager->setFilesList(collectProjectData());
}


namespace {
    constexpr auto kOpenPrefix   = "📄 File opened";
    constexpr auto kCreatePrefix = "✨ File created";
}


void UIManager::openInTarget(UI::ProjectManager* currentManager,
                             const UI::FileSystem::ProjectData& data,
                             bool newWindow,
                             const QString& actionPrefix)
{
    UI::ProjectManager* target = currentManager;

    if (newWindow || !target) {
        target = createEditorProjectManager(new RenderEngine(), new CustomConsole());
        target->setActiveToolEditorWindow(UI::ToolsType::Size);
    }

    if (!target->editor()) {
        target->createEditorWindow(new RenderEngine(), new CustomConsole(), &settingsSystem_, fs_.projectsPath(), fs_.scriptsPath());
    }

    UI::FileData fd;
    fd.id   = data.id;
    fd.name = data.name;
    target->addTabSlot(fd);

    target->addNotification(actionPrefix
                            + (newWindow ? " in new window: " : ": ")
                            + data.name);
}


void UIManager::openFile(qint64 fileId,
                         UI::ProjectManager* currentManager,
                         bool newWindow)
{
    UI::FileSystem::ProjectData data;
    if (fs_.getProject(fileId, data) != UI::FileSystem::FsResult::Ok) {
        if (currentManager) {
            currentManager->addNotification(
                "❌ File not found: id " + QString::number(fileId));
        }
        return;
    }

    openInTarget(currentManager, data, newWindow, kOpenPrefix);
}


void UIManager::openFileByPath(const QString& path,
                               UI::ProjectManager* currentManager,
                               bool newWindow)
{
    if (path.trimmed().isEmpty()) return;

    qint64 fileId = -1;
    const auto res = fs_.registerProject(path, fileId);
    if (res != UI::FileSystem::FsResult::Ok
        && res != UI::FileSystem::FsResult::AlreadyExists) {
        if (currentManager) {
            currentManager->addNotification(
                "❌ Failed to open file: " + QFileInfo(path).fileName());
        }
        return;
    }

    openFile(fileId, currentManager, newWindow);
}


void UIManager::createFile(const QString& fileName,
                           UI::ProjectManager* currentManager,
                           bool newWindow)
{
    qint64 fileId = -1;
    const auto res = fs_.createProject(fileName, fileId);

    switch (res) {
    case UI::FileSystem::FsResult::Ok:
        break;
    case UI::FileSystem::FsResult::AlreadyExists:
        if (currentManager) {
            currentManager->addNotification("❌ File already exists: " + fileName);
        }
        return;
    case UI::FileSystem::FsResult::InvalidArgument:
        if (currentManager) {
            currentManager->addNotification("❌ Invalid file name: " + fileName);
        }
        return;
    default:
        if (currentManager) {
            currentManager->addNotification("❌ Failed to create file: " + fileName);
        }
        return;
    }

    UI::FileSystem::ProjectData data;
    if (fs_.getProject(fileId, data) != UI::FileSystem::FsResult::Ok) return;

    openInTarget(currentManager, data, newWindow, kCreatePrefix);
}


void UIManager::renameFile(UI::FileData& data) {
    if (data.name.isEmpty()) return;

    if (fs_.renameProject(data.id, data.name) == UI::FileSystem::FsResult::Ok) {
        for (auto* m : projectManagers_) {
            m->refreshFilesRequested(collectProjectData());
            m->renameTabSlot(data);
        }
        return;
    }
}


void UIManager::deleteFile(UI::ProjectManager* manager, qint64 fileId) {
    UI::FileSystem::ProjectData data;
    const bool hasData = fs_.getProject(fileId, data) == UI::FileSystem::FsResult::Ok;
    const QString name = hasData ? data.name : QString::number(fileId);

    if (fs_.removeProject(fileId) != UI::FileSystem::FsResult::Ok) {
        manager->addNotification("❌ Failed to remove file: " + name);
        return;
    }

    for (auto* m : projectManagers_) {
        if (m) m->deleteTabSlot(fileId);
    }
    refreshFiles(manager);
    manager->addNotification("🗑️ File removed: " + name);
}


void UIManager::openLightEditor(UI::ProjectManager* manager, UI::FileData& data) {
    if (!manager) return;
    manager->openLightEditorWindow(data, new RenderEngine(), new CustomConsole());
    manager->setActiveToolLightEditorWindow(data.id, UI::ToolsType::Cursor);
}


void UIManager::returnTabFromLight(UI::ProjectManager* manager, qint64 fileId)  {
    if (!manager) return;
    UI::FileSystem::ProjectData data;
    if (fs_.getProject(fileId, data) != UI::FileSystem::FsResult::Ok) return;

    UI::FileData fd;
    fd.id   = data.id;
    fd.name = data.name;
    manager->addTabSlot(fd);
}


void UIManager::setActiveTab(UI::ProjectManager* manager, qint64 fileId) const {
    if (manager) manager->setActiveTab(fileId);
}


void UIManager::goToStartWindow(UI::ProjectManager* manager)  {
    if (!manager) return;
    manager->openStartWindow(collectProjectData(), fs_.projectsPath());
}


QVector<UI::ProjectData> UIManager::collectProjectData()  {
    QVector<UI::ProjectData> files;

    for (const auto& p : fs_.projects()) {
        UI::ProjectData pd;
        pd.id   = p.id;
        pd.name = p.name;
        pd.path = p.path;
        files.append(pd);
    }

    return files;
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


void UIManager::applyLanguage(UI::SettingsSystem::Language lang) {
    auto tryLoad = [this](const QString& path) -> bool {
        auto* tr = new QTranslator(this);
        if (!tr->load(path)) {
            delete tr;
            return false;
        }
        // Новый уже загружен → ставим
        qApp->installTranslator(tr);
        if (translator_) {
            qApp->removeTranslator(translator_);
            delete translator_;
        }
        translator_ = tr;
        qDebug() << "[UIManager] Change translation:" << path;
        return true;
    };

    const QString primary =
        lang == UI::SettingsSystem::Language::Russian    ? QStringLiteral(":/translations/app_ru.qm") :
        lang == UI::SettingsSystem::Language::Belarusian ? QStringLiteral(":/translations/app_by.qm") :
                                                           QStringLiteral(":/translations/app_eng.qm");

    if (!tryLoad(primary)) {
        qWarning() << "[UIManager] Failed to load:" << primary;
        if (lang != UI::SettingsSystem::Language::English) {
            qWarning() << "[UIManager] Falling back to English";
            tryLoad(QStringLiteral(":/translations/app_eng.qm"));
        }
    }

    currentLanguage_ = lang;
}

void UIManager::closeTab(const qint64 fileId) {

}

void UIManager::initSignals(UI::ProjectManager& manager) {



    connect(&manager, &UI::ProjectManager::openFileByIDTriggered, this,
        [this, &manager](qint64 fileId, bool newWindow) {
            openFile(fileId, &manager, newWindow);
        });

    connect(&manager, &UI::ProjectManager::openFileTriggered, this,
            [this, &manager](const QString& path, bool newWindow) {
                openFileByPath(path, &manager, newWindow);
            });

    connect(&manager, &UI::ProjectManager::createFileTriggered, this,
            [this, &manager](const QString& fileName, bool newWindow) {
                createFile(fileName, &manager, newWindow);
            });





    connect(&manager, &UI::ProjectManager::renameFileTriggered, this,
            [this](UI::FileData& data) {
                renameFile(data);
            });

    connect(&manager, &UI::ProjectManager::renameFileTriggered, this,
            [this](UI::FileData& data) {
                renameFile(data);
            });

    connect(&manager, &UI::ProjectManager::removeFileTriggered, this,
            [this, &manager](qint64 fileId) {
                deleteFile(&manager, fileId);
            });

    connect(&manager, &UI::ProjectManager::closeFileTriggered, this,
    [this](const qint64 fileId) {
        closeTab(fileId);
    });


    connect(&manager, &UI::ProjectManager::setActiveFileTriggered, this,
            [this, &manager](qint64 fileId) {
                setActiveTab(&manager, fileId);
            });




    connect(&manager, &UI::ProjectManager::goToStartWindowTriggered, this,
            [this, &manager]() {
                goToStartWindow(&manager);
            });

    connect(&manager, &UI::ProjectManager::openLightEditorTriggered, this,
            [this, &manager](qint64 fileId) {
                UI::FileSystem::ProjectData pd;
                if (fs_.getProject(fileId, pd) != UI::FileSystem::FsResult::Ok) return;

                UI::FileData fd;
                fd.id   = pd.id;
                fd.name = pd.name;
                openLightEditor(&manager, fd);
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

    connect(&manager, &UI::ProjectManager::settingsChanged, this,
            [this, &manager]() {
                settingsChangedSlot(&manager);
            });

    connect(&manager, &QObject::destroyed, this,
            [this, &manager] {
                deleteSlot(&manager);
            });
}