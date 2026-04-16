#include "BaseEditorWindow.h"

#include "BaseEditorPage.h"


UI::BaseEditorWindow::BaseEditorWindow(QWidget* parent)
    : BaseWindow(parent) {
}


void UI::BaseEditorWindow::setQOpenGLPainter(QOpenGLWindow* engine) const {
    if (editorPage_) {
        editorPage_->setQOpenGLPainter(engine);
    }
}


void UI::BaseEditorWindow::setQWindowRender(QWindow* engine) const {
    if (editorPage_) {
        editorPage_->setQWindowRender(engine);
    }
}


void UI::BaseEditorWindow::setCommandConsoleEngine(QLineEdit* engine) const {
    if (editorPage_) {
        editorPage_->setCommandConsoleEngine(engine);
    }
}


void UI::BaseEditorWindow::initEditor(BaseEditorPage* editorPage) {
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
