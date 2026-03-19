#include "TopBarTab.h"

#include "SideTool.h"

#include <QLabel>

#include "../../CommandConsole/CommandConsole.h"


UI::TopBarTab::TopBarTab(QWidget* parent)
    : TopBar(parent) {
    createTabWidgets();
    setObjectName(QStringLiteral("TopBarTab"));
}


void UI::TopBarTab::setLeftTool(SideTool* tool) {
    leftTool_ = tool;
}


void UI::TopBarTab::setRightTool(SideTool* tool) {
    rightTool_ = tool;
}


void UI::TopBarTab::setCommandConsole(UI::CommandConsole* console) {
    commandConsole_ = console;
}


void UI::TopBarTab::createTabWidgets() {
    title_ = new QLabel(QStringLiteral("OurPaint"), this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));

    btnConsole_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/console.svg"));
    btnConsole_->setToolTip("Show/size console");

    btnLeft_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/left.svg"));
    btnLeft_->setToolTip("Show/size left.svg panel");

    btnRight_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/right.svg"));
    btnRight_->setToolTip("Show/size right panel");

    addLeftWidget(title_);
    addCenterWidget(btnLeft_);
    addCenterWidget(btnRight_);
    addCenterWidget(btnConsole_);

    connect(btnLeft_, &QPushButton::clicked, this, [this] {
        if (leftTool_) {
            leftTool_->setVisible(!leftTool_->isVisible());
        }
    });

    connect(btnRight_, &QPushButton::clicked, this, [this] {
        if (rightTool_) {
            rightTool_->setVisible(!rightTool_->isVisible());
        }
    });

    connect(btnConsole_, &QPushButton::clicked, this, [this] {
        if (commandConsole_) {
            commandConsole_->setVisible(!commandConsole_->isVisible());
        }
    });
}
