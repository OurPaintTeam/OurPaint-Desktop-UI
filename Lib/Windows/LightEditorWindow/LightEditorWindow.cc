#include "LightEditorWindow.h"

#include <QCloseEvent>

#include "LightEditorPage.h"


UI::LightEditorWindow::LightEditorWindow(const FileData& data, QWidget* parent)
    : BaseEditorWindow(parent), fileData_(data){
    initUI();
}


void UI::LightEditorWindow::initUI() {
    lightEditorPage_ = new LightEditorPage(this,fileData_);
    initEditor(lightEditorPage_);
    setWindowTitle(fileData_.name);
    connect(lightEditorPage_, &LightEditorPage::returnLightEditorWindowTriggered, this, [this] {
        emit returnTabTriggered(fileData_.id);
    });
    connect(lightEditorPage_, &LightEditorPage::settingsChanged, this, &LightEditorWindow::settingsChanged);
}

void UI::LightEditorWindow::setSettings(SettingsSystem *settings) {
    if (settings) {
        lightEditorPage_->setSettings(settings);
    }
}

void UI::LightEditorWindow::setNotificationsEnabled(const bool enable) {
    includeNotificationManager(enable);
}

void UI::LightEditorWindow::setName(const QString& name) {
    setWindowTitle(name);
    fileData_.name=name;
    lightEditorPage_->setName(name);
}


qint64 UI::LightEditorWindow::id() const {
    return fileData_.id;
}

void UI::LightEditorWindow::closeEvent(QCloseEvent* event) {
    BaseEditorWindow::closeEvent(event);
}