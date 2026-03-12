#include "TopBarTab.h"

#include "HideOpenPanelButton.h"
#include "SideTool.h"

#include <QLabel>


UI::TopBarTab::TopBarTab(QWidget* parent)
    : TopBar(parent) {
    createTabWidgets();
}


void UI::TopBarTab::setLeftTool(SideTool* tool) {
    leftTool_ = tool;
    leftTool_->setObjectName(QStringLiteral("leftTool"));
}


void UI::TopBarTab::setRightTool(SideTool* tool) {
    rightTool_ = tool;
    rightTool_->setObjectName(QStringLiteral("rightTool"));
}


void UI::TopBarTab::createTabWidgets() {
    title_ = new QLabel(QStringLiteral("OurPaint"), this);
    title_->setStyleSheet(QStringLiteral("color: #D8D8F6; font-weight: bold;"));
    title_->setObjectName(QStringLiteral("titleOurPaint"));

    btnLeft_ = createPanelButton();
    btnLeft_->setObjectName(QStringLiteral("btnLeft"));

    btnRight_ = createPanelButton();
    btnRight_->setObjectName(QStringLiteral("btnRight"));

    layout_->insertWidget(0, title);
    layout_->insertWidget(1, btnLeft);
    layout_->insertWidget(2, btnRight);
    layout_->setObjectName(QStringLiteral("m_layout"));

    connect(btnLeft_, &HideOpenPanelButton::toggled, this, [this] {
        if (leftTool_) {
            leftTool_->setVisible(!leftTool_->isVisible());
        }
    });

    connect(btnRight_, &HideOpenPanelButton::toggled, this, [this] {
        if (rightTool_) {
            rightTool_->setVisible(!rightTool_->isVisible());
        }
    });
}
