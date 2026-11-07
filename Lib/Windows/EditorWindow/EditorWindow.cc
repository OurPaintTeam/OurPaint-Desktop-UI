#include "EditorWindow.h"

#include <QCloseEvent>

#include "EditorPage.h"


UI::EditorWindow::EditorWindow(QWidget* parent)
    : BaseEditorWindow(parent) {
    initUI();
}


void UI::EditorWindow::initUI() {
    projectPage_ = new EditorPage(this);
    initEditor(projectPage_);

    connect(projectPage_, &EditorPage::goToStartWindowTriggered,
            this, &EditorWindow::goToStartWindowTriggered);
    connect(projectPage_, &EditorPage::openFileThisWindowTriggered,
            this, &EditorWindow::openFileThisWindowTriggered);
    connect(projectPage_, &EditorPage::createFileThisWindowTriggered,
            this, &EditorWindow::createFileThisWindowTriggered);
    connect(projectPage_, &EditorPage::openNewWindowOpenFileTriggered,
            this, &EditorWindow::openNewWindowOpenFileTriggered);
    connect(projectPage_, &EditorPage::openNewWindowCreateFileTriggered,
            this, &EditorWindow::openNewWindowCreateFileTriggered);
    connect(projectPage_, &EditorPage::openFileTriggered,
            this, &EditorWindow::openFileTriggered);
    connect(projectPage_, &EditorPage::createFileTriggered,
            this, &EditorWindow::createFileTriggered);
    connect(projectPage_, &EditorPage::renameTabTriggered,
            this, &EditorWindow::renameTabTriggered);
    connect(projectPage_, &EditorPage::setActiveTabTriggered,
           this, &EditorWindow::setActiveTabTriggered);
    connect(projectPage_, &EditorPage::removeTabTriggered,
            this, &EditorWindow::removeTabTriggered);
    connect(projectPage_, &EditorPage::openLightEditorWindowTriggered,
            this, &EditorWindow::openLightEditorWindowTriggered);
    connect(projectPage_, &EditorPage::closeApplicationTriggered,
            this, &EditorWindow::closeApplicationTriggered);
  connect(projectPage_, &EditorPage::applySettings,
        this, &EditorWindow::applySettings);
    connect(projectPage_, &EditorPage::scriptTriggered,
      this, &EditorWindow::onOpenScriptTriggered);
}


void UI::EditorWindow::setDefaultPath(const QString& path) {
    if (projectPage_) {
        projectPage_->setDefaultPath(path);
    }
}


void UI::EditorWindow::addTabSlot(const QString& name,const qint64 id) const {
    if (projectPage_) {
        projectPage_->onAddTabSlot(name,id);
    }
}


void UI::EditorWindow::deleteTabSlot( const qint64 id) const {
    if (projectPage_) {
        projectPage_->onDeleteTabSlot(id);
    }
}


void UI::EditorWindow::renameTabSlot(const qint64 id, const QString& newName) const {
    if (projectPage_) {
        projectPage_->onRenameTabSlot(id, newName);
        projectPage_->setActiveID(id);
    }
}


void UI::EditorWindow::closeEvent(QCloseEvent* event) {
    emit windowClosedTriggered();
    BaseEditorWindow::closeEvent(event);
}
