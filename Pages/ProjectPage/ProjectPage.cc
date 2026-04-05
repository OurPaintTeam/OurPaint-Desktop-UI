#include "ProjectPage.h"

#include <QFileDialog>
#include <QStackedWidget>
#include <QSurfaceFormat>

#include "CommandConsole.h"
#include "ContainerWidget.h"
#include "InputWidget.h"
#include "NameInputWidget.h"
#include "NavigationWidget.h"
#include "PainterWidget.h"
#include "PathInputWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "ToolBar.h"
#include "ToolsType.h"
#include "TopBarProject.h"

namespace {
    constexpr int MARGIN = 0;
    constexpr int SPACING = 0;
} // namespace

UI::ProjectPage::ProjectPage(QWidget* parent)
    : QWidget(parent) {
    setObjectName("ProjectPage");
    setAttribute(Qt::WA_StyledBackground);

    initUI();
    setupConnections();
}


void UI::ProjectPage::setQOpenGLPainter(QOpenGLWindow* engine) const {
    painter_->setQOpenGL(engine);
}


void UI::ProjectPage::setQWindowRender(QWindow* engine) const {
    painter_->setQWindowRender(engine);
}


void UI::ProjectPage::setCommandConsoleEngine(QLineEdit* engine) const {
    console_->setLineEditEngine(engine);
}


void UI::ProjectPage::setDefaultProjectsPath(const QString& projectPath) {
    projectDefaultPath_ = projectPath;
}


void UI::ProjectPage::initUI() {
    createLayouts();
    createTopBar();
    createSideColumns();
    createCenterColumn();
    createWorkspace();
    createNavigation();
    createSideContainers();
    assembleLayout();
    updatePageSlot();
}


void UI::ProjectPage::setupConnections() {
    connect(tabBar_, &TabBar::openTabWindowTriggered, this, &UI::ProjectPage::openTabWindowTriggered);
    connect(tabBar_, &TabBar::setActiveTabTriggered, this, &UI::ProjectPage::setActiveTabTriggered);
    connect(tabBar_, &TabBar::createTabTriggered, this, &UI::ProjectPage::createFileTriggered);
    connect(tabBar_, &TabBar::allTabsCloseTriggered, this, &UI::ProjectPage::updatePageSlot);
    connect(tabBar_, &TabBar::renameTabTriggered, this, &UI::ProjectPage::renameTabTriggered);
    connect(tabBar_, &TabBar::removeTabTriggered, this, &UI::ProjectPage::removeTabTriggered);

    connect(navigationPage_, &NavigationWidget::closeApplicationTriggered, this,
            &UI::ProjectPage::closeApplicationTriggered);
    connect(navigationPage_, &NavigationWidget::createProjectTriggered, this, &UI::ProjectPage::onCreateProjectSlot);
    connect(navigationPage_, &NavigationWidget::goToStartWindowTriggered, this,
            &UI::ProjectPage::goToStartWindowTriggered);
    connect(navigationPage_, &NavigationWidget::createFileTriggered, this, &UI::ProjectPage::createFileSlot);


    connect(infoButton_, &QPushButton::clicked, this, &UI::ProjectPage::openInformationPanel);
    connect(messengerButton_, &QPushButton::clicked, this, &UI::ProjectPage::openMessengerPanel);

    /// Tools - constrains
    connect(toolBar_, &ToolBar::pointLineDistanceTriggered,
            this, &UI::ProjectPage::pointLineDistanceTriggered);

    connect(toolBar_, &ToolBar::pointOnLineTriggered,
            this, &UI::ProjectPage::pointOnLineTriggered);

    connect(toolBar_, &ToolBar::pointPointDistanceTriggered,
            this, &UI::ProjectPage::pointPointDistanceTriggered);

    connect(toolBar_, &ToolBar::coincidentPointsTriggered,
            this, &UI::ProjectPage::coincidentPointsTriggered);

    connect(toolBar_, &ToolBar::lineCircleDistanceTriggered,
            this, &UI::ProjectPage::lineCircleDistanceTriggered);

    connect(toolBar_, &ToolBar::lineOnCircleTriggered,
            this, &UI::ProjectPage::lineOnCircleTriggered);

    connect(toolBar_, &ToolBar::lineInCircleTriggered,
            this, &UI::ProjectPage::lineInCircleTriggered);

    connect(toolBar_, &ToolBar::parallelLinesTriggered,
            this, &UI::ProjectPage::parallelLinesTriggered);

    connect(toolBar_, &ToolBar::perpendicularLinesTriggered,
            this, &UI::ProjectPage::perpendicularLinesTriggered);

    connect(toolBar_, &ToolBar::angleBetweenLinesTriggered,
            this, &UI::ProjectPage::angleBetweenLinesTriggered);

    /// Tools - point
    connect(toolBar_, &ToolBar::pointTriggered, this, &UI::ProjectPage::pointTriggered);
    connect(toolBar_, &ToolBar::lineTriggered, this, &UI::ProjectPage::lineTriggered);
    connect(toolBar_, &ToolBar::polylineTriggered, this, &UI::ProjectPage::polylineTriggered);
    connect(toolBar_, &ToolBar::infiniteLineTriggered, this, &UI::ProjectPage::infiniteLineTriggered);

    /// Tools - circle
    connect(toolBar_, &ToolBar::circleByDiameterTriggered, this, &UI::ProjectPage::circleByDiameterTriggered);
    connect(toolBar_, &ToolBar::circleTwoPointsTriggered, this, &UI::ProjectPage::circleTwoPointsTriggered);
    connect(toolBar_, &ToolBar::ellipseThreePointsTriggered, this, &UI::ProjectPage::ellipseThreePointsTriggered);

    /// Tools - arc
    connect(toolBar_, &ToolBar::arcByRadiusTriggered, this, &UI::ProjectPage::arcByRadiusTriggered);
    connect(toolBar_, &ToolBar::arcByDiameterTriggered, this, &UI::ProjectPage::arcByDiameterTriggered);
    connect(toolBar_, &ToolBar::arcByThreePointsTriggered, this, &UI::ProjectPage::arcByThreePointsTriggered);

    /// Tools - line/circle/arc settings
    connect(toolBar_, &ToolBar::lineSettingsTriggered, this, &UI::ProjectPage::lineSettingsTriggered);
    connect(toolBar_, &ToolBar::circleSettingsTriggered, this, &UI::ProjectPage::circleSettingsTriggered);
    connect(toolBar_, &ToolBar::arcSettingsTriggered, this, &UI::ProjectPage::arcSettingsTriggered);

    /// Tools - cursor/size
    connect(toolBar_, &ToolBar::cursorTriggered, this, &UI::ProjectPage::cursorTriggered);
    connect(toolBar_, &ToolBar::sizeTriggered, this, &UI::ProjectPage::sizeTriggered);

    // TopBar - Project
    connect(topBar_, &TopBarProject::createFileTriggered,
            this, &UI::ProjectPage::createFileSlot);

    connect(topBar_, &TopBarProject::openFileTriggered,
            this, &UI::ProjectPage::openFileSlot);

    connect(topBar_, &TopBarProject::createProjectTriggered,
            this, &UI::ProjectPage::onCreateProjectSlot);

    connect(topBar_, &TopBarProject::openProjectTriggered,
            this, &UI::ProjectPage::onOpenProjectSlot);

    connect(topBar_, &TopBarProject::exportFileTriggered,
            this, &UI::ProjectPage::exportFileTriggered);

    connect(topBar_, &TopBarProject::scriptTriggered,
            this, &UI::ProjectPage::scriptTriggered);

    // TopBar - Collaboration
    connect(topBar_, &TopBarProject::collaborationOpenTriggered,
            this, &UI::ProjectPage::collaborationOpenTriggered);

    connect(topBar_, &TopBarProject::collaborationCloseTriggered,
            this, &UI::ProjectPage::collaborationCloseTriggered);

    // TopBar - Version Control
    connect(topBar_, &TopBarProject::versionInitTriggered,
            this, &UI::ProjectPage::versionInitTriggered);

    connect(topBar_, &TopBarProject::versionCreateTriggered,
            this, &UI::ProjectPage::versionCreateTriggered);

    connect(topBar_, &TopBarProject::versionPushTriggered,
            this, &UI::ProjectPage::versionPushTriggered);

    connect(topBar_, &TopBarProject::versionPullTriggered,
            this, &UI::ProjectPage::versionPullTriggered);

    connect(topBar_, &TopBarProject::versionCommitTriggered,
            this, &UI::ProjectPage::versionCommitTriggered);
}


void UI::ProjectPage::createLayouts() {
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    mainLayout_->setSpacing(SPACING);

    mainArea_ = new QWidget(this);

    rootLayout_ = new QHBoxLayout(mainArea_);
    rootLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    rootLayout_->setSpacing(SPACING);
}


void UI::ProjectPage::createTopBar() {
    topBar_ = new TopBarProject(this);
}


void UI::ProjectPage::createSideColumns() {
    leftColumn_ = new SideMenu(this);
    rightColumn_ = new SideMenu(this);
}


void UI::ProjectPage::createCenterColumn() {
    centerColumn_ = new QWidget(this);

    centerLayout_ = new QVBoxLayout(centerColumn_);
    centerLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    centerLayout_->setSpacing(SPACING);

    tabBar_ = new TabBar(centerColumn_);

    centerStack_ = new QStackedWidget(centerColumn_);

    centerLayout_->addWidget(tabBar_);
    centerLayout_->addWidget(centerStack_, 1);
}


void UI::ProjectPage::createWorkspace() {
    workspacePage_ = new QWidget();

    workspaceLayout_ = new QVBoxLayout(workspacePage_);
    workspaceLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    workspaceLayout_->setSpacing(SPACING);

    toolBar_ = new ToolBar(workspacePage_);
    workspaceLayout_->addWidget(toolBar_);

    painterWrapper_ = new QWidget(workspacePage_);

    painterLayout_ = new QVBoxLayout(painterWrapper_);
    constexpr auto margins = 10;
    painterLayout_->setContentsMargins(margins, 0, margins, margins);

    painter_ = new PainterWidget(painterWrapper_);
    painterLayout_->addWidget(painter_);

    workspaceLayout_->addWidget(painterWrapper_, 1);

    consoleWrapper_ = new QWidget(workspacePage_);

    consoleLayout_ = new QHBoxLayout(consoleWrapper_);
    consoleLayout_->setContentsMargins(margins, 0, margins, 0);

    console_ = new CommandConsole(consoleWrapper_);
    consoleLayout_->addWidget(console_);
    connect(console_, &CommandConsole::sentCommandTriggered, this, &ProjectPage::sentCommandTriggered);

    workspaceLayout_->addWidget(consoleWrapper_);

    centerStack_->addWidget(workspacePage_);

    topBar_->setTabBar(tabBar_);
    topBar_->setLeftMenu(leftColumn_);
    topBar_->setRightMenu(rightColumn_);
    topBar_->setConsole(console_);
}


void UI::ProjectPage::createNavigation() {
    navigationPage_ = new NavigationWidget(centerColumn_);

    centerStack_->addWidget(navigationPage_);
}


void UI::ProjectPage::createSideContainers() {
    infoContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    infoButton_ = infoContainer_->addButton(QIcon(":/Assets/icons/panel/info.png"));

    messengerContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    messengerButton_ = messengerContainer_->addButton(QIcon(":/Assets/icons/panel/message.png"));

    leftColumn_->addContainer(infoContainer_);
    leftColumn_->addContainer(messengerContainer_);
}


void UI::ProjectPage::assembleLayout() const {
    mainLayout_->addWidget(topBar_);
    mainLayout_->addWidget(mainArea_, 1);

    rootLayout_->addWidget(leftColumn_);
    rootLayout_->addWidget(centerColumn_, 1);
    rootLayout_->addWidget(rightColumn_);
}


void UI::ProjectPage::createFileSlot() {
    auto* prompt = new NameInputWidget("File Name:", this);

    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);

    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this,prompt](const QString& fileName) {
        if (fileName.isEmpty()) {
            return;
        }
        prompt->deleteLater();
        emit createFileTriggered(fileName);
    });
}


void UI::ProjectPage::updatePageSlot() const {
    if (tabBar_->isEmptyTab()) {
        centerStack_->setCurrentWidget(navigationPage_);
        toolBar_->hideAllElements();
        topBar_->setConsoleButtonEnabled(false);
    } else {
        centerStack_->setCurrentWidget(workspacePage_);
        toolBar_->showAllElements();
        topBar_->setConsoleButtonEnabled(true);
    }
}


void UI::ProjectPage::openInformationPanel() const {
    if (infoContainer_->currentSlot() == leftColumn_->slotWidget()) {
        leftColumn_->openInfPanel();
    } else {
        rightColumn_->openInfPanel();
    }
}


void UI::ProjectPage::openMessengerPanel() const {
    if (messengerContainer_->currentSlot() == leftColumn_->slotWidget()) {
        leftColumn_->openMesPanel();
    } else {
        rightColumn_->openMesPanel();
    }
}


void UI::ProjectPage::onCreateProjectSlot() {
    auto* prompt = new PathInputWidget("Project Path:",this);
    prompt->setCheckBoxQuestion("New Window");
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);
    prompt->setDefaultText(projectDefaultPath_);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this,prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }
        prompt->deleteLater();
        if (prompt->isCheckBoxChecked()) {
            openNewWindowCreateProjectTriggered(text);
        } else {
            createProjectThisWindowTriggered(text);
        }
    });
}


void UI::ProjectPage::onOpenProjectSlot() {
    auto* prompt = new PathInputWidget("Project Path:",this);
    prompt->setCheckBoxQuestion("New Window");
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();
    prompt->setDefaultText(projectDefaultPath_);
    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);

    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this,prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }
        prompt->deleteLater();
        if (prompt->isCheckBoxChecked()) {
            openNewWindowOpenProjectTriggered(text);
        } else {
            openProjectThisWindowTriggered(text);
        }
    });
}


void UI::ProjectPage::openFileSlot() {
    const auto filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File or Project"),
        projectDefaultPath_,
        tr("Project Files (*.ourp);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    emit openFileTriggered(filePath);
}


void UI::ProjectPage::onAddTabSlot(const QString& name) const {
    if (name.isEmpty()) {
        return;
    }
    tabBar_->addTab(name);
    updatePageSlot();
}


void UI::ProjectPage::onDeleteTabSlot(const QString& name) const {
    if (name.isEmpty()) {
        return;
    }
    tabBar_->deleteTabSlot(name);
    updatePageSlot();
}


void UI::ProjectPage::onRenameTabSlot(const QString& oldName, const QString& newName) const {
    if (oldName.isEmpty() || newName.isEmpty()) {
        return;
    }
    tabBar_->renameTabTriggered(oldName, newName);
}
