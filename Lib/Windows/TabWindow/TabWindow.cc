#include "TabWindow.h"

#include <QCloseEvent>

#include "TabPage.h"


UI::TabWindow::TabWindow(const QString& tabName, QWidget* parent)
    : BaseEditorWindow(parent), tabName_(tabName) {
    initUI();
}


void UI::TabWindow::initUI() {
    tabPage_ = new TabPage(this,tabName_);
    initEditor(tabPage_);
    setWindowTitle(tabName_);
    connect(tabPage_,&TabPage::returnTabWindowTriggered,this,&TabWindow::returnTabTriggered);
}


QString UI::TabWindow::tabName() const {
    return tabName_;
}


void UI::TabWindow::setTabName(const QString& tabName) {
    tabName_ = tabName;
    setWindowTitle(tabName_);
    setActiveName(tabName_);
}


void UI::TabWindow::closeEvent(QCloseEvent* event) {
    BaseEditorWindow::closeEvent(event);
}
