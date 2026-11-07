#include "LightEditorWindow.h"

#include <QCloseEvent>

#include "LightEditorPage.h"


UI::LightEditorWindow::LightEditorWindow(qint64 fileId, const QString& tabName, QWidget* parent)
    : BaseEditorWindow(parent), fileId_(fileId), fileName_(tabName) {
    initUI();
}


void UI::LightEditorWindow::initUI() {
    lightEditorPage_ = new LightEditorPage(this,fileId_, fileName_);
    initEditor(lightEditorPage_);
    setWindowTitle(fileName_);
    connect(lightEditorPage_, &LightEditorPage::returnLightEditorWindowTriggered, this, [this] {
        emit returnTabTriggered(fileId_);
    });
}


void UI::LightEditorWindow::setName(const QString& name) {
    setWindowTitle(name);
    fileName_=name;
    lightEditorPage_->setName(name);
}


qint64 UI::LightEditorWindow::id() const {
    return fileId_;
}

void UI::LightEditorWindow::closeEvent(QCloseEvent* event) {
    BaseEditorWindow::closeEvent(event);
}