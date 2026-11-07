#include "LightEditorPage.h"

#include <QVBoxLayout>
#include <utility>

#include "SideTool.h"
#include "TopBarLightEditor.h"


UI::LightEditorPage::LightEditorPage(QWidget* parent, const qint64& id, QString  displayName)
    : BaseEditorPage(parent,id),displayName_(std::move(displayName)) {
    setObjectName(QStringLiteral("TabPage"));
    initUI();
}

void UI::LightEditorPage::setName(const QString& name) {
    topBar_->setDisplayName(name);
    displayName_=name;
}

void UI::LightEditorPage::initUI() {
    initBaseEditorPage();

    topBar_ = new TopBarLightEditor(this);
    topBar_->setDisplayName(displayName_);
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
}