#include "TabPage.h"

#include <QVBoxLayout>

#include "SideTool.h"
#include "TopBarTab.h"


UI::TabPage::TabPage(QWidget* parent)
    : BaseEditorPage(parent) {
    setObjectName(QStringLiteral("TabPage"));
    initUI();
}


void UI::TabPage::initUI() {
    initBaseEditorPage();

    topBar_ = new TopBarTab(this);
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
}
