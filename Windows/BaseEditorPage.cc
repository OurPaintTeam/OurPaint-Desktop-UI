#include "BaseEditorPage.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWindow>

#include "CommandConsole.h"
#include "../Widgets/PainterWidget.h"
#include "ToolBar.h"

namespace {
    constexpr int LAYOUT_MARGIN = 0;
    constexpr int LAYOUT_SPACING = 0;
    constexpr int PADDING_HORIZONTAL = 10;
    constexpr int PADDING_BOTTOM = 10;
} // namespace


UI::BaseEditorPage::BaseEditorPage(QWidget* parent)
    : BasePage(parent) {
}


void UI::BaseEditorPage::setQOpenGLPainter(QOpenGLWindow* engine) const {
    if (painter_) {
        painter_->setQOpenGL(engine);
    }
}

void UI::BaseEditorPage::setQWindowRender(QWindow* engine) const {
    if (painter_) {
        painter_->setQWindowRender(engine);
    }
}


void UI::BaseEditorPage::setCommandConsoleEngine(QLineEdit* engine) const {
    if (console_) {
        console_->setLineEditEngine(engine);
    }
}


void UI::BaseEditorPage::initBaseEditorPage() {
    initBasePage();

    rootLayout_ = new QHBoxLayout(mainArea_);
    rootLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    rootLayout_->setSpacing(LAYOUT_SPACING);
}


QWidget* UI::BaseEditorPage::createWorkspacePage(QWidget* parent) {
    auto* workspacePage = new QWidget(parent);
    workspacePage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    workspaceLayout_ = new QVBoxLayout(workspacePage);
    workspaceLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    workspaceLayout_->setSpacing(LAYOUT_SPACING);

    toolBar_ = new ToolBar(workspacePage);
    toolBar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    workspaceLayout_->addWidget(toolBar_);

    painterWrapper_ = new QWidget(workspacePage);
    painterWrapper_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    painterLayout_ = new QVBoxLayout(painterWrapper_);
    painterLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    painterLayout_->setSpacing(LAYOUT_SPACING);

    painter_ = new PainterWidget(painterWrapper_);
    painterLayout_->addWidget(painter_);
    workspaceLayout_->addWidget(painterWrapper_, 1);

    consoleWrapper_ = new QWidget(workspacePage);
    consoleWrapper_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    consoleWrapper_->setMinimumHeight(0);
    consoleLayout_ = new QHBoxLayout(consoleWrapper_);
    consoleLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    consoleLayout_->setSpacing(LAYOUT_SPACING);

    console_ = new CommandConsole(consoleWrapper_);
    consoleLayout_->addWidget(console_);
    workspaceLayout_->addWidget(consoleWrapper_);
    workspaceLayout_->setStretchFactor(toolBar_, 0);
    workspaceLayout_->setStretchFactor(painterWrapper_, 1);
    workspaceLayout_->setStretchFactor(consoleWrapper_, 0);

    connect(console_, &CommandConsole::sentCommandTriggered,
            this, &BaseEditorPage::sentCommandTriggered);
    connect(toolBar_, &ToolBar::primitiveTriggered,
            this, &BaseEditorPage::primitiveTriggered);
    connect(toolBar_, &ToolBar::constraintTriggered,
            this, &BaseEditorPage::constraintTriggered);
    connect(toolBar_, &ToolBar::toolsTriggered,
            this, &BaseEditorPage::toolsTriggered);

    return workspacePage;
}
