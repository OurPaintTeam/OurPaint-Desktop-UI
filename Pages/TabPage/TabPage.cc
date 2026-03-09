#include "TabPage.h"

#include "TopBarTab.h"
#include "ToolBar.h"
#include "QTPainter.h"
#include "CommandConsole.h"
#include "SideTool.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QSizePolicy>

namespace
{
constexpr qint32 LAYOUT_MARGIN = 0;
constexpr qint32 LAYOUT_SPACING = 0;
constexpr qint32 PADDING_HORIZONTAL = 10;
constexpr qint32 PADDING_BOTTOM = 10;
}


TabPage::TabPage(QWidget* parent)
    : FramelessWindow(parent)
{
    setObjectName(QStringLiteral("tabPage"));
    TabPage::initUI();
}


TabPage::~TabPage() = default;


void TabPage::initUI()
{
    // Central widget
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    setCentralWidget(centralWidget);

    // Main vertical layout
    m_mainLayout = new QVBoxLayout(centralWidget);
    m_mainLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_mainLayout->setSpacing(LAYOUT_SPACING);

    // Top bar
    m_topBar = new TopBarTab(centralWidget);
    m_topBar->setObjectName(QStringLiteral("topBar"));
    m_mainLayout->addWidget(m_topBar);

    // Main area wrapper
    m_mainArea = new QWidget(centralWidget);
    m_mainArea->setObjectName(QStringLiteral("mainArea"));

    m_rootLayout = new QHBoxLayout(m_mainArea);
    m_rootLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_rootLayout->setSpacing(LAYOUT_SPACING);

    m_mainLayout->addWidget(m_mainArea);

    // Side tools
    m_leftTool = new SideTool(m_mainArea);
    m_leftTool->setObjectName(QStringLiteral("leftTool"));

    m_rightTool = new SideTool(m_mainArea);
    m_rightTool->setObjectName(QStringLiteral("rightTool"));

    // Center stack
    m_centerStack = new QWidget(m_mainArea);
    m_centerStack->setObjectName(QStringLiteral("centerStack"));

    m_centerLayout = new QVBoxLayout(m_centerStack);
    m_centerLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_centerLayout->setSpacing(LAYOUT_SPACING);

    // Top toolbar
    m_toolBar = new ToolBar(m_centerStack);
    m_toolBar->setObjectName(QStringLiteral("toolBar"));
    m_centerLayout->addWidget(m_toolBar);

    // Painter area
    m_painterWrapper = new QWidget(m_centerStack);
    m_painterWrapper->setObjectName(QStringLiteral("painterWrapper"));
    m_painterWrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_painterLayout = new QVBoxLayout(m_painterWrapper);
    m_painterLayout->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    m_painterLayout->setSpacing(LAYOUT_SPACING);

    m_painter = new QTPainter(m_painterWrapper);
    m_painter->setObjectName(QStringLiteral("painter"));
    m_painterLayout->addWidget(m_painter);

    m_centerLayout->addWidget(m_painterWrapper, 1);

    // Command console
    m_consoleWrapper = new QWidget(m_centerStack);
    m_consoleWrapper->setObjectName(QStringLiteral("consoleWrapper"));

    m_consoleLayout = new QHBoxLayout(m_consoleWrapper);
    m_consoleLayout->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    m_consoleLayout->setSpacing(LAYOUT_SPACING);

    m_console = new CommandConsole(m_consoleWrapper);
    m_console->setObjectName(QStringLiteral("console"));
    m_consoleLayout->addWidget(m_console);

    m_centerLayout->addWidget(m_consoleWrapper);

    // Assemble root layout
    m_rootLayout->addWidget(m_leftTool);
    m_rootLayout->addWidget(m_centerStack, 1);
    m_rootLayout->addWidget(m_rightTool);
}