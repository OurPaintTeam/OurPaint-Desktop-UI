#include "ProjectWindow.h"

#include <QCloseEvent>

#include "ProjectPage.h"


UI::ProjectWindow::ProjectWindow(QWidget* parent)
    : BaseEditorWindow(parent) {
    initUI();
}


void UI::ProjectWindow::initUI() {
    projectPage_ = new ProjectPage(this);
    initEditor(projectPage_);

    connect(projectPage_, &ProjectPage::goToStartWindowTriggered,
            this, &ProjectWindow::goToStartWindowTriggered);
    connect(projectPage_, &ProjectPage::openProjectThisWindowTriggered,
            this, &ProjectWindow::openProjectThisWindowTriggered);
    connect(projectPage_, &ProjectPage::createProjectThisWindowTriggered,
            this, &ProjectWindow::createProjectThisWindowTriggered);
    connect(projectPage_, &ProjectPage::openNewWindowOpenProjectTriggered,
            this, &ProjectWindow::openNewWindowOpenProjectTriggered);
    connect(projectPage_, &ProjectPage::openNewWindowCreateProjectTriggered,
            this, &ProjectWindow::openNewWindowCreateProjectTriggered);
    connect(projectPage_, &ProjectPage::openFileTriggered,
            this, &ProjectWindow::openFileTriggered);
    connect(projectPage_, &ProjectPage::createFileTriggered,
            this, &ProjectWindow::createFileTriggered);
    connect(projectPage_, &ProjectPage::renameTabTriggered,
            this, &ProjectWindow::renameTabTriggered);
    connect(projectPage_, &ProjectPage::setActiveTabTriggered,
           this, &ProjectWindow::setActiveTabTriggered);
    connect(projectPage_, &ProjectPage::removeTabTriggered,
            this, &ProjectWindow::removeTabTriggered);
    connect(projectPage_, &ProjectPage::openTabWindowTriggered,
            this, &ProjectWindow::openTabWindowTriggered);
    connect(projectPage_, &ProjectPage::closeApplicationTriggered,
            this, &ProjectWindow::closeApplicationTriggered);
  connect(projectPage_, &ProjectPage::applySettings,
        this, &ProjectWindow::applySettings);
    connect(projectPage_, &ProjectPage::scriptTriggered,
      this, &ProjectWindow::onOpenScriptTriggered);
}


void UI::ProjectWindow::setDefaultProjectsPath(const QString& projectPath) {
    if (projectPage_) {
        projectPage_->setDefaultProjectsPath(projectPath);
    }
}


void UI::ProjectWindow::addTabSlot(const QString& name) const {
    if (projectPage_) {
        projectPage_->onAddTabSlot(name);
    }
}


void UI::ProjectWindow::deleteTabSlot(const QString& name) const {
    if (projectPage_) {
        projectPage_->onDeleteTabSlot(name);
    }
}


void UI::ProjectWindow::renameTabSlot(const QString& oldName, const QString& newName) const {
    if (projectPage_) {
        projectPage_->onRenameTabSlot(oldName, newName);
        projectPage_->setActiveName(newName);
    }
}


void UI::ProjectWindow::closeEvent(QCloseEvent* event) {
    emit windowClosedTriggered();
    BaseEditorWindow::closeEvent(event);
}
