#include "TopBarProject.h"

#include <QEvent>

#include "CollaborationMenu.h"
#include "MenuButton.h"
#include "SettingsWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "VersionControlMenu.h"

#include <QToolButton>

#include "ProjectMenu.h"


UI::TopBarProject::TopBarProject(QWidget *parent)
    : BaseEditorTopBar(parent) {
    setObjectName(QStringLiteral("TopBarProject"));
    createProjectButtons();
    translate();
}


void UI::TopBarProject::setTabBar(TabBar *bar) {
    if (!bar) {
        return;
    }
    tabBar_ = bar;
    btnTabs_ = createToggleButton(QIcon(":/Assets/icons/showHidePanels/tab.svg"),
                                  "",
                                  [this]() {
                                      if (tabBar_) {
                                          toggleWidgetVisibility(tabBar_);
                                      }
                                  });
    btnTabs_->setVisible(false);
}


void UI::TopBarProject::setLeftMenu(SideMenu *menu) {
    if (!menu) {
        return;
    }
    setLeftPanel(menu);
}


void UI::TopBarProject::setRightMenu(SideMenu *menu) {
    if (!menu) {
        return;
    }
    setRightPanel(menu);
}


void UI::TopBarProject::setConsole(QWidget *console) {
    if (!console) {
        return;
    }
    BaseEditorTopBar::setConsole(console);
}


void UI::TopBarProject::setTabsButtonEnabled(const bool enabled) const {
    if (btnTabs_) {
        btnTabs_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setLeftButtonEnabled(const bool enabled) const {
    if (leftButton_) {
        leftButton_->setEnabled(enabled);
    }
}


void UI::TopBarProject::setRightButtonEnabled(const bool enabled) const {
    if (rightButton_) {
        rightButton_->setEnabled(enabled);
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
    projectButton_ = new MenuButton("", projectMenu_, this);
    projectButton_->setDraw(true);
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
    collabButton_ = new MenuButton("", collaborationMenu_, this);
    collabButton_->setDraw(true);
    addLeftWidget(collabButton_);
    connect(collaborationMenu_, &UI::CollaborationMenu::openTriggered,
            this, &TopBarProject::collaborationOpenTriggered);

    connect(collaborationMenu_, &UI::CollaborationMenu::closeTriggered,
            this, &TopBarProject::collaborationCloseTriggered);


    versionControlMenu_ = new VersionControlMenu(this);
    verButton_ = new MenuButton("", versionControlMenu_, this);
    verButton_->setDraw(true);


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
    helpButton_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addLeftWidget(helpButton_);

    setupSettings();
}


void UI::TopBarProject::setupSettings() {
    btnSettings_ = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings_->setObjectName(QStringLiteral("btnSettings"));
    constexpr auto btnSize = QSize(32, 32);
    btnSettings_->setFixedSize(btnSize);
    btnSettings_->setFocusPolicy(Qt::NoFocus);

    addCenterWidget(btnSettings_);
    connect(btnSettings_, &QPushButton::clicked, this, [this]() {
        auto *const settings = new SettingsWidget(this);
        settings->setAttribute(Qt::WA_DeleteOnClose);
        settings->show();

        connect(settings, &SettingsWidget::settingsApplied, this, &TopBarProject::applySettings);
    });
}


void UI::TopBarProject::changeEvent(QEvent *e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}


void UI::TopBarProject::translate() const {
    if (projectButton_) {
        projectButton_->setToolTip(UI::TopBarProject::tr("Project menu"));
        projectButton_->setText(UI::TopBarProject::tr("Project"));
    }
    if (verButton_) {
        verButton_->setToolTip(UI::TopBarProject::tr("Version menu"));
        verButton_->setText(UI::TopBarProject::tr("Version control"));
    }
    if (projectMenu_) {
        collabButton_->setToolTip(UI::TopBarProject::tr("Collaboration menu"));
        collabButton_->setText(UI::TopBarProject::tr("Collaboration"));
    }
    if (btnSettings_) {
        btnSettings_->setToolTip(UI::TopBarProject::tr("Settings"));
    }
    if (helpButton_) {
        helpButton_->setToolTip(UI::TopBarProject::tr("Help"));
        helpButton_->setText(UI::TopBarProject::tr("Help"));
    }
    if (btnTabs_) {
        btnTabs_->setToolTip(UI::TopBarProject::tr("Show/Hide tab panel"));
    }
}
