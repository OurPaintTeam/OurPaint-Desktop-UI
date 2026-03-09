#include "TopBarTab.h"

#include "SideTool.h"
#include "HideOpenPanelButton.h"

#include <QLabel>

TopBarTab::TopBarTab(QWidget* parent)
    : TopBar(parent)
{
    createTabWidgets();
}

void TopBarTab::setLeftTool(SideTool* tool)
{
    leftTool = tool;
    leftTool->setObjectName(QStringLiteral("leftTool"));
}

void TopBarTab::setRightTool(SideTool* tool)
{
    rightTool = tool;
    rightTool->setObjectName(QStringLiteral("rightTool"));
}

void TopBarTab::createTabWidgets()
{
    title = new QLabel(QStringLiteral("OurPaint"), this);
    title->setStyleSheet(QStringLiteral("color: #D8D8F6; font-weight: bold;"));
    title->setObjectName(QStringLiteral("titleOurPaint"));

    btnLeft  = createPanelButton();
    btnLeft->setObjectName(QStringLiteral("btnLeft"));

    btnRight = createPanelButton();
    btnRight->setObjectName(QStringLiteral("btnRight"));

    m_layout->insertWidget(0, title);
    m_layout->insertWidget(1, btnLeft);
    m_layout->insertWidget(2, btnRight);
    m_layout->setObjectName(QStringLiteral("m_layout"));

    connect(btnLeft, &HideOpenPanelButton::toggled, this, [this] {
        if (leftTool) {
            leftTool->setVisible(!leftTool->isVisible());
        }
    });

    connect(btnRight, &HideOpenPanelButton::toggled, this, [this] {
        if (rightTool) {
            rightTool->setVisible(!rightTool->isVisible());
        }
    });
}