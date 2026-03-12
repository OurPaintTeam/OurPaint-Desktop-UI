#include "TabPage.h"

#include "CommandConsole.h"
#include "PainterWidget.h"
#include "SideTool.h"
#include "ToolBar.h"
#include "TopBarTab.h"

#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
constexpr qint32 LAYOUT_MARGIN = 0;
constexpr qint32 LAYOUT_SPACING = 0;
constexpr qint32 PADDING_HORIZONTAL = 10;
constexpr qint32 PADDING_BOTTOM = 10;
} // namespace


UI::TabPage::TabPage(QWidget* parent)
    : FramelessWindow(parent)
{
    setObjectName(QStringLiteral("tabPage"));
    TabPage::initUI();
}


UI::TabPage::~TabPage() = default;


void UI::TabPage::initUI()
{
    // Central widget
    centralWidget_ = new QWidget(this);
    centralWidget_->setObjectName(QStringLiteral("centralWidget_"));
    setCentralWidget(centralWidget_);

    // Main vertical layout
    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    mainLayout_->setSpacing(LAYOUT_SPACING);

    // Top bar
    topBar_ = new TopBarTab(centralWidget_);
    topBar_->setObjectName(QStringLiteral("topBar_"));
    mainLayout_->addWidget(topBar_);

    // Main area wrapper
    mainArea_ = new QWidget(centralWidget_);
    mainArea_->setObjectName(QStringLiteral("mainArea_"));

    rootLayout_ = new QHBoxLayout(mainArea_);
    rootLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    rootLayout_->setSpacing(LAYOUT_SPACING);

    mainLayout_->addWidget(mainArea_);

    // Side tools
    leftTool_ = new SideTool(mainArea_);
    leftTool_->setObjectName(QStringLiteral("leftTool_"));

    rightTool_ = new SideTool(mainArea_);
    rightTool_->setObjectName(QStringLiteral("rightTool_"));

    // Center stack
    centerStack_ = new QWidget(mainArea_);
    centerStack_->setObjectName(QStringLiteral("centerStack_"));

    centerLayout_ = new QVBoxLayout(centerStack_);
    centerLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    centerLayout_->setSpacing(LAYOUT_SPACING);

    // Top toolbar
    toolBar_ = new ToolBar(centerStack_);
    toolBar_->setObjectName(QStringLiteral("toolBar_"));
    centerLayout_->addWidget(toolBar_);

    // Painter area
    painterWrapper_ = new QWidget(centerStack_);
    painterWrapper_->setObjectName(QStringLiteral("painterWrapper_"));
    painterWrapper_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    painterLayout_ = new QVBoxLayout(painterWrapper_);
    painterLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    painterLayout_->setSpacing(LAYOUT_SPACING);

    painter_ = new UI::PainterWidget(painterWrapper_);
    painter_->setObjectName(QStringLiteral("painter"));
    painterLayout_->addWidget(painter_);

    centerLayout_->addWidget(painterWrapper_, 1);

    // Command console
    consoleWrapper_ = new QWidget(centerStack_);
    consoleWrapper_->setObjectName(QStringLiteral("consoleWrapper_"));

    consoleLayout_ = new QHBoxLayout(consoleWrapper_);
    consoleLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    consoleLayout_->setSpacing(LAYOUT_SPACING);

    console_ = new CommandConsole(consoleWrapper_);
    console_->setObjectName(QStringLiteral("console_"));
    consoleLayout_->addWidget(console_);

    centerLayout_->addWidget(consoleWrapper_);

    // Assemble root layout
    rootLayout_->addWidget(leftTool_);
    rootLayout_->addWidget(centerStack_, 1);
    rootLayout_->addWidget(rightTool_);
}