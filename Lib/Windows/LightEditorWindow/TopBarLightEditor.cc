#include "TopBarLightEditor.h"

#include <QEvent>

#include "SideTool.h"

#include <QLabel>
#include <QPushButton>


UI::TopBarLightEditor::TopBarLightEditor(QWidget* parent)
    : BaseEditorTopBar(parent) {
    createLightEditorWidgets();
    setObjectName(QStringLiteral("TopBarTab"));
    translate();
}

void UI::TopBarLightEditor::setDisplayName(const QString& name) {
    displayName_ = name;
    updateTitle();
}

void UI::TopBarLightEditor::updateTitle() const {
    if (title_) {
        title_->setText(displayName_.isEmpty() ? UI::TopBarLightEditor::tr("OurPaint") : displayName_);
    }
}


void UI::TopBarLightEditor::setLeftTool(SideTool* tool) {
    setLeftPanel(tool);
}


void UI::TopBarLightEditor::setRightTool(SideTool* tool) {
    setRightPanel(tool);
}


void UI::TopBarLightEditor::setConsole(QWidget* console) {
    BaseEditorTopBar::setConsole(console);
}


void UI::TopBarLightEditor::createLightEditorWidgets() {
    title_ = new QLabel((""), this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));

    returnBtn_ = new QPushButton(this);
    connect(returnBtn_, &QPushButton::clicked, this,&TopBarLightEditor::returnLightEditorWindowTriggered);
    returnBtn_->setObjectName("ReturnTab");
    returnBtn_->setIcon( QIcon(":/Assets/icons/showHidePanels/home.svg"));
    addLeftWidget(title_);
    addLeftWidget(returnBtn_);
}


void UI::TopBarLightEditor::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}


void UI::TopBarLightEditor::translate() const {
    if (returnBtn_) {
        returnBtn_->setToolTip(UI::TopBarLightEditor::tr("Return"));
    }
}