#include "ProjectPage.h"

#include <QFileDialog>
#include <QIcon>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "ContainerWidget.h"
#include "InputWidget.h"
#include "NameInputWidget.h"
#include "NavigationWidget.h"
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


UI::ProjectPage::ProjectPage(QWidget* parent,const QString& name)
    : BaseEditorPage(parent,name) {
    setObjectName("ProjectPage");

    initUI();
    setupConnections();
}


void UI::ProjectPage::setDefaultProjectsPath(const QString& projectPath) {
    projectDefaultPath_ = projectPath;
}


void UI::ProjectPage::initUI() {
    initBaseEditorPage();
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
    connect(tabBar_, &TabBar::openTabWindowTriggered, this, &ProjectPage::openTabWindowTriggered);
    connect(tabBar_, &TabBar::setActiveTabTriggered, this, [this](const QString& name) {
        setActiveName(name);
        emit setActiveTabTriggered(name);
    });
    connect(tabBar_, &TabBar::createTabTriggered, this, &ProjectPage::createFileTriggered);
    connect(tabBar_, &TabBar::allTabsCloseTriggered, this, &ProjectPage::updatePageSlot);
    connect(tabBar_, &TabBar::renameTabTriggered, this, &ProjectPage::renameTabTriggered);
    connect(tabBar_, &TabBar::removeTabTriggered, this, &ProjectPage::removeTabTriggered);

    connect(navigationPage_, &NavigationWidget::closeApplicationTriggered, this,
            &ProjectPage::closeApplicationTriggered);
    connect(navigationPage_, &NavigationWidget::createProjectTriggered, this,
            &ProjectPage::onCreateProjectSlot);
    connect(navigationPage_, &NavigationWidget::goToStartWindowTriggered, this,
            &ProjectPage::goToStartWindowTriggered);
    connect(navigationPage_, &NavigationWidget::createFileTriggered, this,
            &ProjectPage::createFileSlot);

    connect(infoButton_, &QPushButton::clicked, this, &ProjectPage::openInformationPanel);
    connect(messengerButton_, &QPushButton::clicked, this, &ProjectPage::openMessengerPanel);

    connect(topBar_, &TopBarProject::createFileTriggered, this, &ProjectPage::createFileSlot);
    connect(topBar_, &TopBarProject::openFileTriggered, this, &ProjectPage::openFileSlot);
    connect(topBar_, &TopBarProject::createProjectTriggered, this, &ProjectPage::onCreateProjectSlot);
    connect(topBar_, &TopBarProject::openProjectTriggered, this, &ProjectPage::onOpenProjectSlot);
    connect(topBar_, &TopBarProject::exportFileTriggered, this, &ProjectPage::exportFileTriggered);
    connect(topBar_, &TopBarProject::scriptTriggered, this, &ProjectPage::onOpenScriptSlot);

    connect(topBar_, &TopBarProject::collaborationOpenTriggered,
            this, &ProjectPage::collaborationOpenTriggered);
    connect(topBar_, &TopBarProject::collaborationCloseTriggered,
            this, &ProjectPage::collaborationCloseTriggered);

    connect(topBar_, &TopBarProject::versionInitTriggered,
            this, &ProjectPage::versionInitTriggered);
    connect(topBar_, &TopBarProject::versionCreateTriggered,
            this, &ProjectPage::versionCreateTriggered);
    connect(topBar_, &TopBarProject::versionPushTriggered,
            this, &ProjectPage::versionPushTriggered);
    connect(topBar_, &TopBarProject::versionPullTriggered,
            this, &ProjectPage::versionPullTriggered);
    connect(topBar_, &TopBarProject::versionCommitTriggered,
            this, &ProjectPage::versionCommitTriggered);

  connect(topBar_, &TopBarProject::applySettings,
        this, &ProjectPage::applySettings);
}


void UI::ProjectPage::createTopBar() {
    topBar_ = new TopBarProject(this);
    mainLayout_->insertWidget(0, topBar_);
}


void UI::ProjectPage::createSideColumns() {
    leftColumn_  = new SideMenu(mainArea_);
    rightColumn_ = new SideMenu(mainArea_);
}


void UI::ProjectPage::createCenterColumn() {
    centerWidget_ = new QWidget(mainArea_);

    centerLayout_ = new QVBoxLayout(centerWidget_);
    centerLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    centerLayout_->setSpacing(SPACING);

    tabBar_ = new TabBar(centerWidget_);
    centerStack_ = new QStackedWidget(centerWidget_);

    centerLayout_->addWidget(tabBar_);
    centerLayout_->addWidget(centerStack_, 1);
}


void UI::ProjectPage::createWorkspace() {
    workspacePage_ = createWorkspacePage(centerStack_);
    centerStack_->addWidget(workspacePage_);

    topBar_->setTabBar(tabBar_);
    topBar_->setLeftMenu(leftColumn_);
    topBar_->setRightMenu(rightColumn_);
    topBar_->setConsole(consoleWrapper_);
}


void UI::ProjectPage::createNavigation() {
    navigationPage_ = new NavigationWidget(centerWidget_);
    centerStack_->addWidget(navigationPage_);
}


void UI::ProjectPage::createSideContainers() {
    infoContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    infoButton_ = infoContainer_->addButton(QIcon(":/Assets/icons/panel/info.png"));
    infoButton_->setObjectName("InfoButton");

    messengerContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    messengerButton_ = messengerContainer_->addButton(QIcon(":/Assets/icons/panel/message.png"));
    messengerButton_->setObjectName("MesButton");

    leftColumn_->addContainer(infoContainer_);
    leftColumn_->addContainer(messengerContainer_);
}


void UI::ProjectPage::assembleLayout() const {
    rootLayout_->addWidget(leftColumn_);
    rootLayout_->addWidget(centerWidget_, 1);
    rootLayout_->addWidget(rightColumn_);
}

void UI::ProjectPage::createFileSlot() {
    tabBar_->createTabSlot();
}


void UI::ProjectPage::updatePageSlot() const {
    if (tabBar_->isEmptyTab()) {
        centerStack_->setCurrentWidget(navigationPage_);
        toolBar_->hideAllElements();
        topBar_->setConsoleButtonEnabled(false);
        return;
    }

    centerStack_->setCurrentWidget(workspacePage_);
    toolBar_->showAllElements();
    topBar_->setConsoleButtonEnabled(true);
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
    auto* prompt = new PathInputWidget("Project Path:", this);
    prompt->setCheckBoxQuestion("New Window");

    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);
    prompt->setDefaultText(projectDefaultPath_);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this, prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }

        prompt->deleteLater();
        if (prompt->isCheckBoxChecked()) {
            emit openNewWindowCreateProjectTriggered(text);
        } else {
            emit createProjectThisWindowTriggered(text);
        }
    });
}


void UI::ProjectPage::onOpenScriptSlot() {
    auto* prompt = new PathInputWidget("Script Path:", this,true);
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->setDefaultText(projectDefaultPath_);
    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this, prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }

        prompt->deleteLater();
        emit scriptTriggered(text);
    });
}


void UI::ProjectPage::onOpenProjectSlot() {
    auto* prompt = new PathInputWidget("Project Path:", this);
    prompt->setCheckBoxQuestion("New Window");

    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->setDefaultText(projectDefaultPath_);
    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this, prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }

        prompt->deleteLater();
        if (prompt->isCheckBoxChecked()) {
            emit openNewWindowOpenProjectTriggered(text);
        } else {
            emit openProjectThisWindowTriggered(text);
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

    tabBar_->addTabSlot(name);
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

    tabBar_->renameTab(oldName, newName);
}
