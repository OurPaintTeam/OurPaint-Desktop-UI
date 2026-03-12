#include "ProjectPage.h"

#include "CommandConsole.h"
#include "PainterWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "ToolBar.h"
#include "TopBarProject.h"

#include <QSizePolicy>
#include <QVBoxLayout>

#include "DraggleContainer.h"

namespace {
    constexpr qint32 LAYOUT_MARGIN = 0;
    constexpr qint32 LAYOUT_SPACING = 0;
    constexpr qint32 PADDING_HORIZONTAL = 10;
    constexpr qint32 PADDING_BOTTOM = 10;
} // namespace


UI::ProjectPage::ProjectPage(QWidget* parent)
    : QWidget(parent) {
    setObjectName(QStringLiteral("projectPage"));
    setStyleSheet(QStringLiteral("background: #978897;"));
    initUI();
}


void UI::ProjectPage::initUI() {
    // Main vertical layout
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    mainLayout_->setSpacing(LAYOUT_SPACING);

    // Main area container
    mainArea_ = new QWidget(this);
    mainArea_->setObjectName(QStringLiteral("mainArea"));

    rootContentLayout_ = new QHBoxLayout(mainArea_);
    rootContentLayout_->setObjectName(QStringLiteral("rootContentLayout"));
    rootContentLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    rootContentLayout_->setSpacing(LAYOUT_SPACING);

    mainLayout_->addWidget(mainArea_);

    // Center column
    centerStack_ = new QWidget(mainArea_);
    centerStack_->setObjectName(QStringLiteral("centerStack"));

    centerLayout_ = new QVBoxLayout(centerStack_);
    centerLayout_->setObjectName(QStringLiteral("centerLayout"));
    centerLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    centerLayout_->setSpacing(LAYOUT_SPACING);

    // Top tab bar
    tabBar_ = new TabBar(centerStack_);
    tabBar_->setObjectName(QStringLiteral("tabBar"));
    connect(tabBar_, &TabBar::openTabWindow, this, &ProjectPage::requestOpenTabPage);

    // Top toolbar
    topToolBar_ = new ToolBar(centerStack_);
    topToolBar_->setObjectName(QStringLiteral("topToolBar"));
    centerLayout_->addWidget(tabBar_);
    centerLayout_->addWidget(topToolBar_);

    // Painter area
    painterWrapper_ = new QWidget(centerStack_);
    painterWrapper_->setObjectName(QStringLiteral("painterWrapper"));
    painterWrapper_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    painterLayout_ = new QVBoxLayout(painterWrapper_);
    painterLayout_->setObjectName(QStringLiteral("painterLayout"));
    painterLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, PADDING_BOTTOM);
    painterLayout_->setSpacing(LAYOUT_SPACING);

    painter_ = new PainterWidget(painterWrapper_);
    painter_->setObjectName(QStringLiteral("painter"));
    painterLayout_->addWidget(painter_);

    centerLayout_->addWidget(painterWrapper_, 1);

    // Command console
    commandConsoleWrapper_ = new QWidget(centerStack_);
    commandConsoleWrapper_->setObjectName(QStringLiteral("commandConsoleWrapper"));

    commandConsoleLayout_ = new QHBoxLayout(commandConsoleWrapper_);
    commandConsoleLayout_->setContentsMargins(PADDING_HORIZONTAL, 0, PADDING_HORIZONTAL, 0);
    commandConsoleLayout_->setSpacing(LAYOUT_SPACING);

    commandConsole_ = new CommandConsole(commandConsoleWrapper_);
    commandConsole_->setObjectName(QStringLiteral("commandConsole"));
    commandConsoleLayout_->addWidget(commandConsole_);

    centerLayout_->addWidget(commandConsoleWrapper_);

    // Side columns
    leftColumn_ = new SideMenu(this);
    leftColumn_->setObjectName(QStringLiteral("leftColumn"));

    rightColumn_ = new SideMenu(this);
    rightColumn_->setObjectName(QStringLiteral("rightColumn"));

    topBar_ = new TopBarProject(this);
    topBar_->setObjectName(QStringLiteral("topBarProject"));
    mainLayout_->insertWidget(0, topBar_);

    // Side info/messenger containers
    infoContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    infoContainer_->setObjectName(QStringLiteral("infoContainer"));
    infoButton_ = infoContainer_->addButton(QStringLiteral("I"));
    infoButton_->setObjectName(QStringLiteral("infoButton"));

    messengerContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    messengerContainer_->setObjectName(QStringLiteral("messengerContainer"));

    messengerButton_ = messengerContainer_->addButton(QStringLiteral("M"));
    messengerButton_->setObjectName(QStringLiteral("messengerButton"));

    connect(infoButton_, &QPushButton::clicked, this, [this]() {
        if (infoContainer_->currentSlot() == leftColumn_->slotWidget()) {
            leftColumn_->openInfPanel();
        } else {
            rightColumn_->openInfPanel();
        }
    });

    connect(messengerButton_, &QPushButton::clicked, this, [this]() {
        if (messengerContainer_->currentSlot() == leftColumn_->slotWidget()) {
            leftColumn_->openMesPanel();
        } else {
            rightColumn_->openMesPanel();
        }
    });

    leftColumn_->addContainer(infoContainer_);
    leftColumn_->addContainer(messengerContainer_);

    // Assemble root layout
    rootContentLayout_->addWidget(leftColumn_);
    rootContentLayout_->addWidget(centerStack_, 1);
    rootContentLayout_->addWidget(rightColumn_);
}
