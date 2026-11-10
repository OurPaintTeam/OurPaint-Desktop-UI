#include "EditorWindow.h"

#include <QCloseEvent>

#include "EditorPage.h"


namespace UI {
    struct FileData;
}


UI::EditorWindow::EditorWindow(QWidget* parent)
    : BaseEditorWindow(parent){
    initUI();
}


void UI::EditorWindow::initUI() {
    editorPage_ = new EditorPage(this);
    initEditor(editorPage_);

    connect(editorPage_, &EditorPage::goToStartWindowTriggered,
            this, &EditorWindow::goToStartWindowTriggered);

    connect(editorPage_, &EditorPage::openFileTriggered,
            this, &EditorWindow::openFileTriggered);
    connect(editorPage_, &EditorPage::createFileTriggered,
            this, &EditorWindow::createFileTriggered);

    connect(editorPage_, &EditorPage::renameTabTriggered,
            this, &EditorWindow::renameTabTriggered);
    connect(editorPage_, &EditorPage::setActiveTabTriggered,
           this, &EditorWindow::setActiveTabTriggered);
    connect(editorPage_, &EditorPage::removeTabTriggered,
            this, &EditorWindow::removeTabTriggered);
    connect(editorPage_, &EditorPage::closeTabTriggered,
this, &EditorWindow::closeTabTriggered);

    connect(editorPage_, &EditorPage::openLightEditorWindowTriggered,
        this, &EditorWindow::openLightEditorWindowTriggered);
  connect(editorPage_, &EditorPage::settingsChanged,
        this, &EditorWindow::settingsChanged);
    connect(editorPage_, &EditorPage::scriptTriggered,
      this, &EditorWindow::onOpenScriptTriggered);

    connect(editorPage_, &EditorPage::closeApplicationTriggered,
            this, &EditorWindow::closeApplicationTriggered);
}


void UI::EditorWindow::setDefaultPath(const QString& path) {
    if (editorPage_) {
        editorPage_->setDefaultPath(path);
    }
}


void UI::EditorWindow::addTabSlot(const FileData& data) const {
    if (editorPage_) {
        editorPage_->onAddTabSlot(data);
    }
}


void UI::EditorWindow::deleteTabSlot( const qint64 id) const {
    if (editorPage_) {
        editorPage_->onDeleteTabSlot(id);
    }
}


void UI::EditorWindow::renameTabSlot(FileData& data) const {
    if (editorPage_) {
        editorPage_->onRenameTabSlot(data);
        editorPage_->setActiveID(data.id);
    }
}


void UI::EditorWindow::closeEvent(QCloseEvent* event) {
    emit closeApplicationTriggered();
    BaseEditorWindow::closeEvent(event);
}


void UI::EditorWindow::applyLanguage(UI::SettingsSystem::Language lang) {

}