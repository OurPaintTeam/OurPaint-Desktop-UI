#include "TopBarTab.h"

#include "SideTool.h"

#include <QLabel>
#include <QPushButton>


UI::TopBarTab::TopBarTab(QWidget* parent)
    : BaseEditorTopBar(parent) {
    createTabWidgets();
    setObjectName(QStringLiteral("TopBarTab"));
}


void UI::TopBarTab::setLeftTool(SideTool* tool) {
    setLeftPanel(tool, QIcon(":/Assets/icons/showHidePanels/left.svg"),
                 QStringLiteral("Show/size left.svg panel"));
}


void UI::TopBarTab::setRightTool(SideTool* tool) {
    setRightPanel(tool, QIcon(":/Assets/icons/showHidePanels/right.svg"),
                  QStringLiteral("Show/size right panel"));
}


void UI::TopBarTab::setConsole(QWidget* console) {
    BaseEditorTopBar::setConsole(console);
}


void UI::TopBarTab::createTabWidgets() {
    title_ = new QLabel(QStringLiteral("OurPaint"), this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));

    const auto x = new QPushButton(this);
    connect(x, &QPushButton::clicked, this,&TopBarTab::returnTabWindowTriggered);
    x->setToolTip("Return");
    x->setObjectName("ReturnTab");
    x->setIcon( QIcon(":/Assets/icons/showHidePanels/home.svg"));
    addLeftWidget(title_);
    addLeftWidget(x);
}
