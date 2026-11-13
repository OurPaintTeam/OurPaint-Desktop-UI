#include "LightEditorPage.h"

#include <QVBoxLayout>
#include <utility>

#include "SideTool.h"
#include "TopBarLightEditor.h"
#include "Utils.h"


UI::LightEditorPage::LightEditorPage(QWidget* parent, FileData data)
    : BaseEditorPage(parent),fileData_(data) {
    setObjectName(QStringLiteral("TabPage"));
    initUI();
    setActiveID(data.id);
}

void UI::LightEditorPage::setName(const QString& name) {
    topBar_->setDisplayName(name);
    fileData_.name=name;
}

void UI::LightEditorPage::setSettings(SettingsSystem *settings) {
    if (settings) {
        topBar_->setSettings(settings);
    }
}

void UI::LightEditorPage::initUI() {
    initBaseEditorPage();

    topBar_ = new TopBarLightEditor(this);
    topBar_->setDisplayName(fileData_.name);
    mainLayout_->insertWidget(0, topBar_);

    leftTool_ = new SideTool(mainArea_);
    rightTool_ = new SideTool(mainArea_);

    centerWidget_ = createWorkspacePage(mainArea_);

    rootLayout_->addWidget(leftTool_);
    rootLayout_->addWidget(centerWidget_, 1);
    rootLayout_->addWidget(rightTool_);

    topBar_->setConsole(consoleWrapper_);
    topBar_->setLeftTool(leftTool_);
    topBar_->setRightTool(rightTool_);

    connect(topBar_,&TopBarLightEditor::returnLightEditorWindowTriggered,this,[this] {
        emit returnLightEditorWindowTriggered();
    });
    connect(topBar_, &TopBarLightEditor::settingsChanged, this, &LightEditorPage::settingsChanged);
    leftTool_->hide();
    rightTool_->hide();
    topBar_->setConsoleButtonEnabled(false);
}