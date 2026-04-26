#include "TopBarTab.h"

#include <QEvent>

#include "SideTool.h"

#include <QLabel>
#include <QPushButton>


UI::TopBarTab::TopBarTab(QWidget* parent)
    : BaseEditorTopBar(parent) {
    createTabWidgets();
    setObjectName(QStringLiteral("TopBarTab"));
    translate();
}


void UI::TopBarTab::setLeftTool(SideTool* tool) {
    setLeftPanel(tool);
}


void UI::TopBarTab::setRightTool(SideTool* tool) {
    setRightPanel(tool);
}


void UI::TopBarTab::setConsole(QWidget* console) {
    BaseEditorTopBar::setConsole(console);
}


void UI::TopBarTab::createTabWidgets() {
    title_ = new QLabel((""), this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));

    returnBtn_ = new QPushButton(this);
    connect(returnBtn_, &QPushButton::clicked, this,&TopBarTab::returnTabWindowTriggered);
    returnBtn_->setObjectName("ReturnTab");
    returnBtn_->setIcon( QIcon(":/Assets/icons/showHidePanels/home.svg"));
    addLeftWidget(title_);
    addLeftWidget(returnBtn_);
}


void UI::TopBarTab::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::TopBarTab::translate() const {
    if (title_) {
        title_->setText(UI::TopBarTab::tr("OurPaint"));
    }
    if (returnBtn_) {
        returnBtn_->setToolTip(UI::TopBarTab::tr("Return"));
    }
}