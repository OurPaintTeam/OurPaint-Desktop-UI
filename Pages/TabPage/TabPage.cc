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



    /// Tools - constrains
    connect(toolBar_, &ToolBar::pointLineDistanceTriggered,
            this, &UI::TabPage::pointLineDistanceTriggered);

    connect(toolBar_, &ToolBar::pointOnLineTriggered,
            this, &UI::TabPage::pointOnLineTriggered);

    connect(toolBar_, &ToolBar::pointPointDistanceTriggered,
            this, &UI::TabPage::pointPointDistanceTriggered);

    connect(toolBar_, &ToolBar::coincidentPointsTriggered,
            this, &UI::TabPage::coincidentPointsTriggered);

    connect(toolBar_, &ToolBar::lineCircleDistanceTriggered,
            this, &UI::TabPage::lineCircleDistanceTriggered);

    connect(toolBar_, &ToolBar::lineOnCircleTriggered,
            this, &UI::TabPage::lineOnCircleTriggered);

    connect(toolBar_, &ToolBar::lineInCircleTriggered,
            this, &UI::TabPage::lineInCircleTriggered);

    connect(toolBar_, &ToolBar::parallelLinesTriggered,
            this, &UI::TabPage::parallelLinesTriggered);

    connect(toolBar_, &ToolBar::perpendicularLinesTriggered,
            this, &UI::TabPage::perpendicularLinesTriggered);

    connect(toolBar_, &ToolBar::angleBetweenLinesTriggered,
            this, &UI::TabPage::angleBetweenLinesTriggered);

    /// Tools - point
    connect(toolBar_, &ToolBar::pointTriggered, this, &UI::TabPage::pointTriggered);
    connect(toolBar_, &ToolBar::lineTriggered, this, &UI::TabPage::lineTriggered);
    connect(toolBar_, &ToolBar::polylineTriggered, this, &UI::TabPage::polylineTriggered);
    connect(toolBar_, &ToolBar::infiniteLineTriggered, this, &UI::TabPage::infiniteLineTriggered);

    /// Tools - circle
    connect(toolBar_, &ToolBar::circleByDiameterTriggered, this, &UI::TabPage::circleByDiameterTriggered);
    connect(toolBar_, &ToolBar::circleTwoPointsTriggered, this, &UI::TabPage::circleTwoPointsTriggered);
    connect(toolBar_, &ToolBar::ellipseThreePointsTriggered, this, &UI::TabPage::ellipseThreePointsTriggered);

    /// Tools - arc
    connect(toolBar_, &ToolBar::arcByRadiusTriggered, this, &UI::TabPage::arcByRadiusTriggered);
    connect(toolBar_, &ToolBar::arcByDiameterTriggered, this, &UI::TabPage::arcByDiameterTriggered);
    connect(toolBar_, &ToolBar::arcByThreePointsTriggered, this, &UI::TabPage::arcByThreePointsTriggered);

    /// Tools - line/circle/arc settings
    connect(toolBar_, &ToolBar::lineSettingsTriggered, this, &UI::TabPage::lineSettingsTriggered);
    connect(toolBar_, &ToolBar::circleSettingsTriggered, this, &UI::TabPage::circleSettingsTriggered);
    connect(toolBar_, &ToolBar::arcSettingsTriggered, this, &UI::TabPage::arcSettingsTriggered);

    /// Tools - cursor/size
    connect(toolBar_, &ToolBar::cursorTriggered, this, &UI::TabPage::cursorTriggered);
    connect(toolBar_, &ToolBar::sizeTriggered, this, &UI::TabPage::sizeTriggered);
}
