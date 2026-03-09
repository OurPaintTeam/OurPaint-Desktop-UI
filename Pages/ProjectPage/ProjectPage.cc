#include "ProjectPage.h"

#include "TopBarProject.h"
#include "TabBar.h"
#include "ToolBar.h"
#include "QTPainter.h"
#include "CommandConsole.h"
#include "SideMenu.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QSizePolicy>

namespace
{
constexpr qint32 LAYOUT_MARGIN = 0;
constexpr qint32 LAYOUT_SPACING = 0;
constexpr qint32 PADDING_HORIZONTAL = 10;
constexpr qint32 PADDING_BOTTOM = 10;
}

ProjectPage::ProjectPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("projectPage"));
    setStyleSheet(QStringLiteral("background: #978897;"));
    initUI();
}

void ProjectPage::initUI()
{
    // Main vertical layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_mainLayout->setSpacing(LAYOUT_SPACING);

    // Main area container
    m_mainArea = new QWidget(this);
    m_mainArea->setObjectName(QStringLiteral("mainArea"));

    m_rootContentLayout = new QHBoxLayout(m_mainArea);
    m_rootContentLayout->setObjectName(QStringLiteral("m_rootContentLayout"));
    m_rootContentLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_rootContentLayout->setSpacing(LAYOUT_SPACING);

    m_mainLayout->addWidget(m_mainArea);

    // Center column
    m_centerStack = new QWidget(m_mainArea);
    m_centerStack->setObjectName(QStringLiteral("centerStack"));

    m_centerLayout = new QVBoxLayout(m_centerStack);
    m_centerLayout->setObjectName(QStringLiteral("m_centerLayout"));
    m_centerLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    m_centerLayout->setSpacing(LAYOUT_SPACING);

    // Top tab bar
    m_tabBar = new TabBar(m_centerStack);
    m_tabBar->setObjectName(QStringLiteral("tabBar"));
    connect(m_tabBar, &TabBar::openTabWindow, this, &ProjectPage::requestOpenTabPage);

    // Top toolbar
    m_topToolBar = new ToolBar(m_centerStack);
    m_topToolBar->setObjectName(QStringLiteral("topToolBar"));
    m_centerLayout->addWidget(m_tabBar);
    m_centerLayout->addWidget(m_topToolBar);

    // Painter area
    m_painterWrapper = new QWidget(m_centerStack);
    m_painterWrapper->setObjectName(QStringLiteral("painterWrapper"));
    m_painterWrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_painterLayout = new QVBoxLayout(m_painterWrapper);
    m_painterLayout->setObjectName(QStringLiteral("m_painterLayout"));
    m_painterLayout->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    m_painterLayout->setSpacing(LAYOUT_SPACING);

    m_painter = new QTPainter(m_painterWrapper);
    m_painter->setObjectName(QStringLiteral("painter"));
    m_painterLayout->addWidget(m_painter);

    m_centerLayout->addWidget(m_painterWrapper, 1);

    // Command console
    m_commandConsoleWrapper = new QWidget(m_centerStack);
    m_commandConsoleWrapper->setObjectName(QStringLiteral("commandConsoleWrapper"));

    m_commandConsoleLayout = new QHBoxLayout(m_commandConsoleWrapper);
    m_commandConsoleLayout->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    m_commandConsoleLayout->setSpacing(LAYOUT_SPACING);

    m_commandConsole = new CommandConsole(m_commandConsoleWrapper);
    m_commandConsole->setObjectName(QStringLiteral("commandConsole"));
    m_commandConsoleLayout->addWidget(m_commandConsole);

    m_centerLayout->addWidget(m_commandConsoleWrapper);

    // Side columns
    m_leftColumn = new SideMenu(this);
    m_leftColumn->setObjectName(QStringLiteral("leftColumn"));

    m_rightColumn = new SideMenu(this);
    m_rightColumn->setObjectName(QStringLiteral("rightColumn"));

    m_topBar = new TopBarProject(this);
    m_topBar->setObjectName(QStringLiteral("topBarProject"));
    m_mainLayout->insertWidget(0, m_topBar);

    // Side info/messenger containers
    infoContainer = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    infoContainer->setObjectName(QStringLiteral("infoContainer"));
    infoButton = infoContainer->addButton(QStringLiteral("I"));
    infoButton->setObjectName(QStringLiteral("infoButton"));

    messengerContainer = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    messengerContainer->setObjectName(QStringLiteral("messengerContainer"));

    messengerButton = messengerContainer->addButton(QStringLiteral("M"));
    messengerButton->setObjectName(QStringLiteral("messengerButton"));

    connect(infoButton, &QPushButton::clicked, this, [this]() {
        if (infoContainer->currentSlot() == m_leftColumn->slotWidget()) {
            m_leftColumn->openInfPanel();
        }
        else {
            m_rightColumn->openInfPanel();
        }
    });

    connect(messengerButton, &QPushButton::clicked, this, [this]() {
        if (messengerContainer->currentSlot() == m_leftColumn->slotWidget()) {
            m_leftColumn->openMesPanel();
        }
        else {
            m_rightColumn->openMesPanel();
        }
    });

    m_leftColumn->addContainer(infoContainer);
    m_leftColumn->addContainer(messengerContainer);

    // Assemble root layout
    m_rootContentLayout->addWidget(m_leftColumn);
    m_rootContentLayout->addWidget(m_centerStack, 1);
    m_rootContentLayout->addWidget(m_rightColumn);
}