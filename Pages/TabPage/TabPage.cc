#include "TabPage.h"

#include "CommandConsole.h"
#include "PainterWidget.h"
#include "SideTool.h"
#include "ToolBar.h"
#include "TopBarTab.h"

#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

namespace {
    constexpr qint32 LAYOUT_MARGIN = 0;
    constexpr qint32 LAYOUT_SPACING = 0;
    constexpr qint32 PADDING_HORIZONTAL = 10;
    constexpr qint32 PADDING_BOTTOM = 10;
} // namespace


UI::TabPage::TabPage(QWidget* parent)
    : FramelessWindow(parent) {
    setObjectName(QStringLiteral("TabPage"));
    setAttribute(Qt::WA_StyledBackground, true);
    TabPage::initUI();
}


UI::TabPage::~TabPage() = default;


void UI::TabPage::initUI() {
    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    // Main vertical layout
    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    mainLayout_->setSpacing(LAYOUT_SPACING);

    // Top bar
    topBar_ = new TopBarTab(centralWidget_);
    mainLayout_->addWidget(topBar_);

    // Main area wrapper
    mainArea_ = new QWidget(centralWidget_);
    mainArea_->setObjectName("MainContent");

    rootLayout_ = new QHBoxLayout(mainArea_);
    rootLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    rootLayout_->setSpacing(LAYOUT_SPACING);

    mainLayout_->addWidget(mainArea_);

    // Side tools
    leftTool_ = new SideTool(mainArea_);

    rightTool_ = new SideTool(mainArea_);

    // Center stack
    centerStack_ = new QWidget(mainArea_);
    centerStack_->setObjectName(QStringLiteral("CenterStack"));

    centerLayout_ = new QVBoxLayout(centerStack_);
    centerLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    centerLayout_->setSpacing(LAYOUT_SPACING);

    // Top toolbar
    toolBar_ = new ToolBar(centerStack_);
    centerLayout_->addWidget(toolBar_);

    // Painter area
    painterWrapper_ = new QWidget(centerStack_);
    painterWrapper_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    painterLayout_ = new QVBoxLayout(painterWrapper_);
    painterLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    painterLayout_->setSpacing(LAYOUT_SPACING);

    painter_ = new UI::PainterWidget(painterWrapper_);
    painterLayout_->addWidget(painter_);

    centerLayout_->addWidget(painterWrapper_, 1);

    // Command console
    consoleWrapper_ = new QWidget(centerStack_);

    consoleLayout_ = new QHBoxLayout(consoleWrapper_);
    consoleLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    consoleLayout_->setSpacing(LAYOUT_SPACING);

    console_ = new CommandConsole(consoleWrapper_);
    connect(console_, &CommandConsole::sentCommandTriggered, this, &TabPage::sentCommandTriggered);
    consoleLayout_->addWidget(console_);

    centerLayout_->addWidget(consoleWrapper_);

    // Assemble root layout
    rootLayout_->addWidget(leftTool_);
    rootLayout_->addWidget(centerStack_, 1);
    rootLayout_->addWidget(rightTool_);

    topBar_->setCommandConsole(console_);
    topBar_->setLeftTool(leftTool_);
    topBar_->setRightTool(rightTool_);


    connect(toolBar_, &ToolBar::primitiveTriggered,
            this, &UI::TabPage::primitiveTriggered);

    connect(toolBar_, &ToolBar::constraintTriggered,
            this, &UI::TabPage::constraintTriggered);

    connect(toolBar_, &ToolBar::toolsTriggered,
            this, &UI::TabPage::toolsTriggered);
}
