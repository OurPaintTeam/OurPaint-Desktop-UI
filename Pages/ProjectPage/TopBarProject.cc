#include "TopBarProject.h"

#include "TabBar.h"
#include "SideMenu.h"
#include "HideOpenPanelButton.h"
#include "SettingsWidget.h"

#include <QPushButton>

TopBarProject::TopBarProject(QWidget* parent)
    : TopBar(parent)
{
    setObjectName(QStringLiteral("TopBarProject"));
    createProjectButtons();
}

void TopBarProject::setTabBar(TabBar* bar)
{
    tabBar = bar;
}

void TopBarProject::setLeftMenu(SideMenu* menu)
{
    leftMenu = menu;
    if (leftMenu) {
        leftMenu->setObjectName(QStringLiteral("leftMenu"));
    }
}

void TopBarProject::setRightMenu(SideMenu* menu)
{
    rightMenu = menu;
    if (rightMenu) {
        rightMenu->setObjectName(QStringLiteral("rightMenu"));
    }
}

void TopBarProject::createProjectButtons()
{
    // Tabs toggle button
    btnTabs = createPanelButton();
    btnTabs->setObjectName(QStringLiteral("btnTabs"));

    // Left panel toggle
    btnLeft = createPanelButton();
    btnLeft->setObjectName(QStringLiteral("btnLeft"));

    // Right panel toggle
    btnRight = createPanelButton();
    btnRight->setObjectName(QStringLiteral("btnRight"));

    // Settings button
    btnSettings = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings->setObjectName(QStringLiteral("btnSettings"));
    btnSettings->setFixedSize(32, 32);
    btnSettings->setFocusPolicy(Qt::NoFocus);
    btnSettings->setStyleSheet(QStringLiteral(R"(
        QPushButton {
            background: transparent;
            color: white;
            border: none;
            border-radius: 6px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.15);
        }
    )"));

    // Add buttons to layout
    m_layout->insertWidget(0, btnTabs);
    m_layout->insertWidget(1, btnLeft);
    m_layout->insertWidget(2, btnRight);
    m_layout->insertWidget(3, btnSettings);

    // Connect signals
    connect(btnTabs, &HideOpenPanelButton::toggled, this, [this]() {
        if (tabBar) {
            tabBar->setVisible(!tabBar->isVisible());
        }
    });

    connect(btnLeft, &HideOpenPanelButton::toggled, this, [this]() {
        if (leftMenu) {
            leftMenu->setVisible(!leftMenu->isVisible());
        }
    });

    connect(btnRight, &HideOpenPanelButton::toggled, this, [this]() {
        if (rightMenu) {
            rightMenu->setVisible(!rightMenu->isVisible());
        }
    });

    connect(btnSettings, &QPushButton::clicked, this, [this]() {
        const auto settings = new SettingsWidget(this);
        settings->setAttribute(Qt::WA_DeleteOnClose);
        settings->setObjectName(QStringLiteral("settingsWidget"));
        settings->show();
    });
}