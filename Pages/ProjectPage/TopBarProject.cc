#include "TopBarProject.h"

#include "CollaborationMenu.h"
#include "CommandConsole.h"
#include "MenuButton.h"
#include "SettingsWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "VersionControlMenu.h"

#include <QPushButton>
#include <QToolButton>

#include "ProjectMenu.h"


UI::TopBarProject::TopBarProject(QWidget* parent)
    : TopBar(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QStringLiteral("TopBarProject"));
    createProjectButtons();
}


void UI::TopBarProject::setTabBar(TabBar* bar) {
    if (!bar) {
        return;
    }
    tabBar_ = bar;
    btnTabs_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/tab.svg"));
    btnTabs_->setToolTip("Show/size tab panel");
    addCenterWidget(btnTabs_);
    connect(btnTabs_, &QPushButton::clicked, this, [this]() {
        if (tabBar_) {
            tabBar_->setVisible(!tabBar_->isVisible());
        }
    });
}


void UI::TopBarProject::setLeftMenu(SideMenu* menu) {
    if (!menu) {
        return;
    }
    leftMenu_ = menu;
    btnLeft_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/left.svg"));
    btnLeft_->setToolTip("Show/size left.svg panel");
    addCenterWidget(btnLeft_);
    connect(btnLeft_, &QPushButton::clicked, this, [this]() {
        if (leftMenu_) {
            leftMenu_->setVisible(!leftMenu_->isVisible());
        }
    });
}


void UI::TopBarProject::setRightMenu(SideMenu* menu) {
    if (!menu) {
        return;
    }
    rightMenu_ = menu;
    btnRight_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/right.svg"));
    btnRight_->setToolTip("Show/size right panel");
    addCenterWidget(btnRight_);
    connect(btnRight_, &QPushButton::clicked, this, [this]() {
        if (rightMenu_) {
            rightMenu_->setVisible(!rightMenu_->isVisible());
        }
    });
}


void UI::TopBarProject::setConsole(CommandConsole* console) {
    if (!console) {
        return;
    }
    commandConsole_ = console;
    btnConsole_ = createWindowButton(QIcon(":/Assets/icons/showHidePanels/console.svg"));

    btnConsole_->setToolTip("Show/size console");
    addCenterWidget(btnConsole_);
    connect(btnConsole_, &QPushButton::clicked, this, [this]() {
        if (commandConsole_) {
            commandConsole_->setVisible(!commandConsole_->isVisible());
        }
    });
}


void UI::TopBarProject::setTabsButtonEnabled(const bool enabled) const {
    if (btnTabs_) {
        btnTabs_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setLeftButtonEnabled(const bool enabled) const {
    if (btnLeft_) {
        btnLeft_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setRightButtonEnabled(const bool enabled) const {
    if (btnRight_) {
        btnRight_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setConsoleButtonEnabled(const bool enabled) const {
    if (btnConsole_) {
        btnConsole_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setPanelsButtonsEnabled(const bool enabled) const {
    setTabsButtonEnabled(enabled);
    setLeftButtonEnabled(enabled);
    setRightButtonEnabled(enabled);
    setConsoleButtonEnabled(enabled);
}


void UI::TopBarProject::createProjectButtons() {
    projectMenu_ = new ProjectMenu(this);
    projectButton_ = new MenuButton("Project", projectMenu_, this);
    projectButton_->setDraw(true);
    projectButton_->setToolTip("Project menu");
    addLeftWidget(projectButton_);
    connect(projectMenu_, &ProjectMenu::createProjectTriggered,
            this, &TopBarProject::createProjectTriggered);

    connect(projectMenu_, &ProjectMenu::openProjectTriggered,
            this, &TopBarProject::openProjectTriggered);

    connect(projectMenu_, &ProjectMenu::createFileTriggered,
            this, &TopBarProject::createFileTriggered);

    connect(projectMenu_, &ProjectMenu::openFileTriggered,
            this, &TopBarProject::openFileTriggered);

    connect(projectMenu_, &ProjectMenu::exportFileTriggered,
            this, &TopBarProject::exportFileTriggered);

    connect(projectMenu_, &ProjectMenu::scriptTriggered,
            this, &TopBarProject::scriptTriggered);


    collaborationMenu_ = new CollaborationMenu(this);
    collabButton_ = new MenuButton("Collaboration", collaborationMenu_, this);
    collabButton_->setDraw(true);
    collabButton_->setToolTip("Collaboration menu");
    addLeftWidget(collabButton_);
    connect(collaborationMenu_, &UI::CollaborationMenu::openTriggered,
            this, &TopBarProject::collaborationOpenTriggered);

    connect(collaborationMenu_, &UI::CollaborationMenu::closeTriggered,
            this, &TopBarProject::collaborationCloseTriggered);


    versionControlMenu_ = new VersionControlMenu(this);
    verButton_ = new MenuButton("Version Control", versionControlMenu_, this);
    verButton_->setDraw(true);

    verButton_->setToolTip("Version menu");
    addLeftWidget(verButton_);
    connect(versionControlMenu_, &UI::VersionControlMenu::initTriggered,
            this, &TopBarProject::versionInitTriggered);

    connect(versionControlMenu_, &UI::VersionControlMenu::createTriggered,
            this, &TopBarProject::versionCreateTriggered);

    connect(versionControlMenu_, &UI::VersionControlMenu::pushTriggered,
            this, &TopBarProject::versionPushTriggered);

    connect(versionControlMenu_, &UI::VersionControlMenu::pullTriggered,
            this, &TopBarProject::versionPullTriggered);

    connect(versionControlMenu_, &UI::VersionControlMenu::commitTriggered,
            this, &TopBarProject::versionCommitTriggered);


    helpButton_ = new QPushButton(this);
    helpButton_->setObjectName(QStringLiteral("HelpButton"));
    helpButton_->setText("Help");
    helpButton_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    helpButton_->setToolTip("Help");
    addLeftWidget(helpButton_);

    setupSettings();
}


void UI::TopBarProject::setupSettings() {
    btnSettings_ = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings_->setObjectName(QStringLiteral("btnSettings"));
    btnSettings_->setToolTip("Settings");
    constexpr auto btnSize = QSize(32, 32);
    btnSettings_->setFixedSize(btnSize);
    btnSettings_->setFocusPolicy(Qt::NoFocus);
    btnSettings_->setObjectName(QStringLiteral("ButtonSettings"));

    addCenterWidget(btnSettings_);
    connect(btnSettings_, &QPushButton::clicked, this, [this]() {
        auto* const settings = new SettingsWidget(this);
        settings->setAttribute(Qt::WA_DeleteOnClose);
        settings->show();
    });
}
