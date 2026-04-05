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


void UI::UIManager::PointLineDistanceSlot(const UI::MainWindow* window) {
    window->addNotification("Point-Line Distance tool selected");
}


void UI::UIManager::PointOnLineSlot(const UI::MainWindow* window) {
    window->addNotification("Point on Line tool selected");
}


void UI::UIManager::PointPointDistanceSlot(const UI::MainWindow* window) {
    window->addNotification("Point-Point Distance tool selected");
}


void UI::UIManager::CoincidentPointsSlot(const UI::MainWindow* window) {
    window->addNotification("Coincident Points tool selected");
}


void UI::UIManager::LineCircleDistanceSlot(const UI::MainWindow* window) {
    window->addNotification("Line-Circle Distance tool selected");
}


void UI::UIManager::LineOnCircleSlot(const UI::MainWindow* window) {
    window->addNotification("Line on Circle tool selected");
}


void UI::UIManager::LineInCircleSlot(const UI::MainWindow* window) {
    window->addNotification("Line in Circle tool selected");
}


void UI::UIManager::ParallelLinesSlot(const UI::MainWindow* window) {
    window->addNotification("Parallel Lines tool selected");
}


void UI::UIManager::PerpendicularLinesSlot(const UI::MainWindow* window) {
    window->addNotification("Perpendicular Lines tool selected");
}


void UI::UIManager::AngleBetweenLinesSlot(const UI::MainWindow* window) {
    window->addNotification("Angle Between Lines tool selected");
}


void UI::UIManager::PointSlot(const UI::MainWindow* window) { window->addNotification("Point tool selected"); }
void UI::UIManager::LineSlot(const UI::MainWindow* window) { window->addNotification("Line tool selected"); }
void UI::UIManager::PolylineSlot(const UI::MainWindow* window) { window->addNotification("Polyline tool selected"); }


void UI::UIManager::InfiniteLineSlot(const UI::MainWindow* window) {
    window->addNotification("Infinite Line tool selected");
}


void UI::UIManager::CircleByDiameterSlot(const UI::MainWindow* window) {
    window->addNotification("Circle by Diameter tool selected");
}


void UI::UIManager::CircleTwoPointsSlot(const UI::MainWindow* window) {
    window->addNotification("Circle by Two Points tool selected");
}


void UI::UIManager::EllipseThreePointsSlot(const UI::MainWindow* window) {
    window->addNotification("Ellipse by Three Points tool selected");
}


void UI::UIManager::ArcByRadiusSlot(const UI::MainWindow* window) {
    window->addNotification("Arc by Radius tool selected");
}


void UI::UIManager::ArcByDiameterSlot(const UI::MainWindow* window) {
    window->addNotification("Arc by Diameter tool selected");
}


void UI::UIManager::ArcByThreePointsSlot(const UI::MainWindow* window) {
    window->addNotification("Arc by Three Points tool selected");
}


void UI::UIManager::LineSettingsSlot(const UI::MainWindow* window) {
    window->addNotification("Line Settings selected");
}


void UI::UIManager::CircleSettingsSlot(const UI::MainWindow* window) {
    window->addNotification("Circle Settings selected");
}


void UI::UIManager::ArcSettingsSlot(const UI::MainWindow* window) { window->addNotification("Arc Settings selected"); }

void UI::UIManager::CursorSlot(const UI::MainWindow* window) { window->addNotification("Cursor tool selected"); }
void UI::UIManager::SizeSlot(const UI::MainWindow* window) { window->addNotification("Size tool selected"); }


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


    // Tools - constrains
    QObject::connect(&window, &UI::MainWindow::pointLineDistanceTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) PointLineDistanceSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::pointOnLineTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) PointOnLineSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::pointPointDistanceTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) PointPointDistanceSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::coincidentPointsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) CoincidentPointsSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::lineCircleDistanceTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) LineCircleDistanceSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::lineOnCircleTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) LineOnCircleSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::lineInCircleTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) LineInCircleSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::parallelLinesTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) ParallelLinesSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::perpendicularLinesTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) PerpendicularLinesSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::angleBetweenLinesTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) AngleBetweenLinesSlot(ptrWindow);
    });

    // Tools - point/line
    QObject::connect(&window, &UI::MainWindow::pointTriggered, this,
                     [this, ptrWindow]() { if (ptrWindow) PointSlot(ptrWindow); });
    QObject::connect(&window, &UI::MainWindow::lineTriggered, this,
                     [this, ptrWindow]() { if (ptrWindow) LineSlot(ptrWindow); });
    QObject::connect(&window, &UI::MainWindow::polylineTriggered, this,
                     [this, ptrWindow]() { if (ptrWindow) PolylineSlot(ptrWindow); });
    QObject::connect(&window, &UI::MainWindow::infiniteLineTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) InfiniteLineSlot(ptrWindow);
    });

    // Tools - circle
    QObject::connect(&window, &UI::MainWindow::circleByDiameterTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) CircleByDiameterSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::circleTwoPointsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) CircleTwoPointsSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::ellipseThreePointsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) EllipseThreePointsSlot(ptrWindow);
    });

    // Tools - arc
    QObject::connect(&window, &UI::MainWindow::arcByRadiusTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) ArcByRadiusSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::arcByDiameterTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) ArcByDiameterSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::arcByThreePointsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) ArcByThreePointsSlot(ptrWindow);
    });

    // Tools - settings
    QObject::connect(&window, &UI::MainWindow::lineSettingsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) LineSettingsSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::circleSettingsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) CircleSettingsSlot(ptrWindow);
    });
    QObject::connect(&window, &UI::MainWindow::arcSettingsTriggered, this, [this, ptrWindow]() {
        if (ptrWindow) ArcSettingsSlot(ptrWindow);
    });

    // Tools - cursor/size
    QObject::connect(&window, &UI::MainWindow::cursorTriggered, this,
                     [this, ptrWindow]() { if (ptrWindow) CursorSlot(ptrWindow); });
    QObject::connect(&window, &UI::MainWindow::sizeTriggered, this,
                     [this, ptrWindow]() { if (ptrWindow) SizeSlot(ptrWindow); });


    // --- Cleanup ---
    QObject::connect(&window, &QObject::destroyed, this, [this](QObject* obj) {
        if (auto* w = qobject_cast<UI::MainWindow*>(obj)) {
            deleteSlot(w);
        }
    });
}
