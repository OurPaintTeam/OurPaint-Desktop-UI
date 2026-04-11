#include "UIManager.h"

#include <QFileInfo>
#include <QLayout>
#include <QPointer>
#include <algorithm>

#include "CustomConsole.h"
#include "MainWindow.h"

#include <QFile>
#include <qiodevice.h>
#include <QWindow>


void dumpObjectTreeToGraphviz(QObject* root, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return;
    }

    QTextStream out(&file);

    out << "digraph QObjectTree {\n";
    out << "    node [shape=box, style=filled, fillcolor=\"#E3F2FD\"];\n";

    std::function<void(QObject*)> recurse = [&](QObject* obj) {
        QString objName = QString("%1_%2")
                .arg(obj->metaObject()->className())
                .arg(reinterpret_cast<quintptr>(obj));

        // Label
        QString label = obj->metaObject()->className();
        if (!obj->objectName().isEmpty()) {
            label += "\\n(" + obj->objectName() + ")";
        }

        out << "    \"" << objName << "\" [label=\"" << label << "\"];\n";

        for (QObject* child: obj->children()) {
            QString childName = QString("%1_%2")
                    .arg(child->metaObject()->className())
                    .arg(reinterpret_cast<quintptr>(child));

            out << "    \"" << objName << "\" -> \"" << childName << "\";\n";

            recurse(child);
        }
    };

    recurse(root);

    out << "}\n";
    file.close();

    qDebug() << "Graphviz file generated:" << filePath;
}


//dumpObjectTreeToGraphviz(window, "full_app.dot");

UI::UIManager::UIManager() {
    setlocale(LC_ALL, "");
    const auto* window = createWindow(nullptr, {});
    window->setProjectsList(fs_.projects());
}


bool UI::UIManager::checkedOpened(const QString& projectPath) const {
    return std::any_of(mainWindows_.begin(), mainWindows_.end(),
                       [&](const UI::MainWindow* t) {
                           return t->projectPath() == projectPath;
                       });
}


void UI::UIManager::sentCommandFromConsole(const UI::MainWindow* window, const QString& command) {
}


void UI::UIManager::closeApplication(const UI::MainWindow* window, const QString& projectPath) {
}


void UI::UIManager::openNewWindowOpenProjectSlot(const UI::MainWindow* window, const QString& projectPath) {
    if (checkedOpened(projectPath)) {
        window->addNotification("Проект уже открыт");
        return;
    }
    if (UI::FileSystem::ProjectData data; fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        auto* newWindow = createWindow(nullptr, {data.path, data.id});
        newWindow->onOpenProjectSlot({data.path, data.id});
        window->setQWindowRender(new QWindow());
        newWindow->setCommandConsoleEngine(new CustomConsole());
        for (const auto& t: data.tabs) {
            newWindow->addTabSlot(t);
        }
        newWindow->addNotification("✅ Project opened in new window: " + data.name);
    }
}


void UI::UIManager::openNewWindowCreateProjectSlot(const UI::MainWindow* window, const QString& projectPath) {
    if (checkedOpened(projectPath)) {
        window->addNotification("Проект уже открыт");
        return;
    }

    const auto projectName = QFileInfo(projectPath).fileName();

    if (QString projectId; fs_.createProject(projectPath, projectId) == UI::FileSystem::FsResult::Ok) {
        auto* newWindow = createWindow(nullptr, {projectPath, projectId});
        newWindow->onOpenProjectSlot({projectPath, projectId});
        window->setQWindowRender(new QWindow());
        newWindow->setCommandConsoleEngine(new CustomConsole());
        newWindow->addNotification("✨ Project created in new window: " + projectName);
    }
}


void UI::UIManager::openProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const {
    if (checkedOpened(projectPath)) {
        window->addNotification("Проект уже открыт: " + projectPath);
    } else if (UI::FileSystem::ProjectData data;
        fs_.openProjectByPath(projectPath, data) == UI::FileSystem::FsResult::Ok) {
        window->onOpenProjectSlot({data.path, data.id});
        window->setQWindowRender(new QWindow());
        window->setCommandConsoleEngine(new CustomConsole());
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
        window->setQWindowRender(new QWindow());
        window->setCommandConsoleEngine(new CustomConsole());
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
    if (FileSystem::ProjectData data; fs_.renameProjectByPath(path, newName, data) == UI::FileSystem::FsResult::Ok) {
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


void UI::UIManager::primitiveSlot(const UI::MainWindow* window, PrimitiveType& type) {
    window->addNotification("Primitive slot selected");
}


void UI::UIManager::constraintSlot(const UI::MainWindow* window, ConstraintType& type) {
    window->addNotification("Constraint slot selected");
}


void UI::UIManager::toolsSlot(const UI::MainWindow* window, ToolsType& type) {
    window->addNotification("Tools slot");
}


UI::MainWindow* UI::UIManager::createWindow(UI::MainWindow* parent, const UI::MainWindow::ProjectData& projectData) {
    auto* window = new UI::MainWindow(parent, projectData);
    window->show();
    mainWindows_.push_back(window);
    window->addNotification("Welcome to OurPaint! 🎨");
    window->setDefaultProjectsPath(fs_.defaultProjectsPath());
    initSignals(*window);

    return window;
}


void UI::UIManager::initSignals(UI::MainWindow& window) {
    QPointer<UI::MainWindow> ptrWindow(&window);


    // Console
    QObject::connect(&window, &UI::MainWindow::sentCommandTriggered, this, [this, ptrWindow](const QString& command) {
        if (ptrWindow) {
            sentCommandFromConsole(ptrWindow, command);
        }
    });

    // --- Open project in new window ---
    QObject::connect(&window, &UI::MainWindow::openNewWindowOpenProjectTriggered,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             openNewWindowOpenProjectSlot(ptrWindow, path);
                         }
                     });

    // --- Create project in new window ---
    QObject::connect(&window, &UI::MainWindow::openNewWindowCreateProjectTriggered,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             openNewWindowCreateProjectSlot(ptrWindow, path);
                         }
                     });

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

    // Close
    QObject::connect(&window, &UI::MainWindow::closeApplication,
                     this, [this, ptrWindow](const QString& path) {
                         if (ptrWindow) {
                             closeApplication(ptrWindow, path);
                         }
                     });


    QObject::connect(&window, &UI::MainWindow::primitiveTriggered, this, [this, ptrWindow](PrimitiveType& type) {
        if (ptrWindow) {
            primitiveSlot(ptrWindow, type);
        }
    });
    QObject::connect(&window, &UI::MainWindow::constraintTriggered, this, [this, ptrWindow](ConstraintType& type) {
        if (ptrWindow) {
            constraintSlot(ptrWindow, type);
        }
    });
    QObject::connect(&window, &UI::MainWindow::toolsTriggered, this, [this, ptrWindow](ToolsType& type) {
        if (ptrWindow) {
            toolsSlot(ptrWindow, type);
        }
    });


    // --- Cleanup ---
    QObject::connect(&window, &QObject::destroyed, this, [this](QObject* obj) {
        if (auto* w = qobject_cast<UI::MainWindow*>(obj)) {
            deleteSlot(w);
        }
    });
}
