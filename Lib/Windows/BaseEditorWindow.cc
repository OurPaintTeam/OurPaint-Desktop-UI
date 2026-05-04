#include "BaseEditorWindow.h"

#include "BaseEditorPage.h"


UI::BaseEditorWindow::BaseEditorWindow(QWidget *parent)
    : BaseWindow(parent) {
}


void UI::BaseEditorWindow::setActiveTool(ToolsType tool) {
    editorPage_->setActiveTool(tool);
}


void UI::BaseEditorWindow::setActiveTool(PrimitiveType tool) {
    editorPage_->setActiveTool(tool);
}


void UI::BaseEditorWindow::setActiveTool(ConstraintType tool) {
    editorPage_->setActiveTool(tool);
}


void UI::BaseEditorWindow::setHintConstraintTools(const QVector<ConstraintType>& vecTools) {
    editorPage_->setHintConstraintTools(vecTools);
}

void UI::BaseEditorWindow::takeOffHint() {
    editorPage_->takeOffHint();
}

void UI::BaseEditorWindow::setActiveName(const QString& name) {
    editorPage_->setActiveName(name);
}


void UI::BaseEditorWindow::setQOpenGLPainter(QOpenGLWindow *engine) const {
    if (editorPage_) {
        editorPage_->setQOpenGLPainter(engine);
    }
}


void UI::BaseEditorWindow::setQWindowRender(QWindow *engine) const {
    if (editorPage_) {
        editorPage_->setQWindowRender(engine);
    }
}


void UI::BaseEditorWindow::setCommandConsoleEngine(QLineEdit *engine) const {
    if (editorPage_) {
        editorPage_->setCommandConsoleEngine(engine);
    }
}


void UI::BaseEditorWindow::initEditor(BaseEditorPage *editorPage) {
    editorPage_ = editorPage;
    setCentralWidget(editorPage_);

    connect(editorPage_, &BaseEditorPage::sentCommandTriggered,
            this, &BaseEditorWindow::sentCommandTriggered);
    connect(editorPage_, &BaseEditorPage::primitiveTriggered,
            this, &BaseEditorWindow::primitiveTriggered);
    connect(editorPage_, &BaseEditorPage::constraintTriggered,
            this, &BaseEditorWindow::constraintTriggered);
    connect(editorPage_, &BaseEditorPage::toolsTriggered,
            this, &BaseEditorWindow::toolsTriggered);

    initNotificationManager();
}
