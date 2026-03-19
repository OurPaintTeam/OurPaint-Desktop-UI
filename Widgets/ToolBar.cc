#include "ToolBar.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QWheelEvent>

#include "ArcMenu.h"
#include "CircleMenu.h"
#include "ContainerWidget.h"
#include "LineMenu.h"
#include "MenuButton.h"
#include "SlotWidget.h"
#include "ToolsType.h"


UI::ToolBar::ToolBar(QWidget* parent)
    : QWidget(parent),
      arcMenu_(new ArcMenu(this)),
      circleMenu_(new CircleMenu(this)),
      lineMenu_(new LineMenu(this)),
      stack_(new QStackedWidget(this)),
      mainLayout_(new QHBoxLayout(this)),
      firstLaneWidget_(new QWidget(this)),
      firstLayout_(new QHBoxLayout(firstLaneWidget_)),
      secondLaneWidget_(new QWidget(this)),
      secondLayout_(new QHBoxLayout(secondLaneWidget_)),
      toolsContainer_(new ContainerWidget(ContainerType::Tools, Qt::Horizontal)),
      figuresSlot_(new SlotWidget({ContainerType::Tools}, Qt::Horizontal)),
      figuresContainer_(new ContainerWidget(ContainerType::Tools, Qt::Horizontal)),
      reqsContainer_(new ContainerWidget(ContainerType::Tools, Qt::Horizontal)),
      isFirstLaneVisible_(true) {
    setObjectName("ToolBar");
    constexpr auto x = 32;
    setFixedHeight(x);
    setAttribute(Qt::WA_StyledBackground, true);

    initLayout();
    initFiguresLane();
    initConstraintsLane();
    initSignals();

    stack_->setCurrentIndex(0);
}


void UI::ToolBar::showAllElements() {
    reqsContainer_->show();
    figuresContainer_->show();
    toolsContainer_->show();
    show();
}


void UI::ToolBar::hideAllElements() {
    reqsContainer_->hide();
    figuresContainer_->hide();
    toolsContainer_->hide();
    hide();
}


void UI::ToolBar::wheelEvent(QWheelEvent* event) {
    event->accept();
}


void UI::ToolBar::handlePrimitiveActionSlot(const QAction* action) {
    if (!action) {
        return;
    }

    switch (action->data().value<PrimitiveType>()) {
        case PrimitiveType::Line:
            emit lineTriggered();
            break;

        case PrimitiveType::Polyline:
            emit polylineTriggered();
            break;

        case PrimitiveType::InfiniteLine:
            emit infiniteLineTriggered();
            break;
        case PrimitiveType::LineSettings:
            emit lineSettingsTriggered();
            break;

        case PrimitiveType::CircleByDiameter:
            emit circleByDiameterTriggered();
            break;

        case PrimitiveType::CircleTwoPoints:
            emit circleTwoPointsTriggered();
            break;

        case PrimitiveType::EllipseThreePoints:
            emit ellipseThreePointsTriggered();
            break;
        case PrimitiveType::CircleSettings:
            emit circleSettingsTriggered();
            break;


        case PrimitiveType::ArcByRadius:
            emit arcByRadiusTriggered();
            break;

        case PrimitiveType::ArcByDiameter:
            emit arcByDiameterTriggered();
            break;

        case PrimitiveType::ArcThreePoints:
            emit arcByThreePointsTriggered();
            break;
        case PrimitiveType::ArcSettings:
            emit arcSettingsTriggered();
            break;
        default: break;
    }
}


void UI::ToolBar::initLayout() const {
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    mainLayout_->addWidget(stack_);

    firstLaneWidget_->setAttribute(Qt::WA_StyledBackground, true);
    secondLaneWidget_->setAttribute(Qt::WA_StyledBackground, true);
}


void UI::ToolBar::initFiguresLane() {
    constexpr int spacing = 10;
    constexpr QSize buttonSize(25, 25);

    firstLayout_->setContentsMargins(0, 0, 0, 0);
    firstLayout_->setSpacing(0);
    firstLayout_->addSpacing(spacing);

    pointPrimitive_ = figuresContainer_->addButton(QIcon(":/Assets/icons/primitives/point.png"));
    pointPrimitive_->setToolTip("Point");

    linePrimitive_ = figuresContainer_->addMenuButton("", lineMenu_);
    circlePrimitive_ = figuresContainer_->addMenuButton("", circleMenu_);
    arcPrimitive_ = figuresContainer_->addMenuButton("", arcMenu_);

    linePrimitive_->setOpenLeftClick(false);
    linePrimitive_->setChange(true);

    circlePrimitive_->setOpenLeftClick(false);
    circlePrimitive_->setChange(true);

    arcPrimitive_->setOpenLeftClick(false);
    arcPrimitive_->setChange(true);


    constexpr QSize toolsSize(25, 25);
    cursorTool_ = toolsContainer_->addButton(QIcon(":/Assets/icons/tools/cursorTool.png"));
    cursorTool_->setFixedSize(toolsSize);
    cursorTool_->setCursor(Qt::PointingHandCursor);
    cursorTool_->setToolTip("Cursor tool");

    sizeTool_ = toolsContainer_->addButton(QIcon(":/Assets/icons/tools/sizeTool.svg"));
    sizeTool_->setFixedSize(toolsSize);
    sizeTool_->setCursor(Qt::PointingHandCursor);
    sizeTool_->setToolTip("Show size");
    sizeTool_->setIconSize(buttonSize);

    firstLayout_->addWidget(figuresSlot_);
    firstLayout_->addSpacing(spacing);
    figuresSlot_->insert(toolsContainer_);
    figuresSlot_->insert(figuresContainer_);

    stack_->addWidget(firstLaneWidget_);
}


void UI::ToolBar::initConstraintsLane() {
    secondLayout_->setContentsMargins(0, 0, 0, 0);
    secondLayout_->setSpacing(0);

    reqsSlot_ = new SlotWidget({ContainerType::Tools}, Qt::Horizontal);
    figuresSlot_->insert(reqsContainer_);

    stack_->addWidget(secondLaneWidget_);
    // isFirstLaneVisible_ = true
}


void UI::ToolBar::initSignals() {
    constexpr QSize buttonSize(25, 25);

    constraints_ = {
        {
            ":/Assets/icons/constraints/pointLineDist.svg", "Point – Line Distance",
            [this] { emit pointLineDistanceTriggered(); }
        },
        {":/Assets/icons/constraints/pointOnLine.svg", "Point on Line", [this] { emit pointOnLineTriggered(); }},
        {
            ":/Assets/icons/constraints/pointPointDist.svg", "Point – Point Distance",
            [this] { emit pointPointDistanceTriggered(); }
        },
        {
            ":/Assets/icons/constraints/pointOnPoint.svg", "Coincident Points",
            [this] { emit coincidentPointsTriggered(); }
        },
        {
            ":/Assets/icons/constraints/lineCircleDist.svg", "Line – Circle Distance",
            [this] { emit lineCircleDistanceTriggered(); }
        },
        {":/Assets/icons/constraints/lineOnCircle.svg", "Line on Circle", [this] { emit lineOnCircleTriggered(); }},
        {":/Assets/icons/constraints/lineInCircle.svg", "Line Inside Circle", [this] { emit lineInCircleTriggered(); }},
        {
            ":/Assets/icons/constraints/lineLineParallel.svg", "Parallel Lines",
            [this] { emit parallelLinesTriggered(); }
        },
        {
            ":/Assets/icons/constraints/lineLinePerpendicular.svg", "Perpendicular Lines",
            [this] { emit perpendicularLinesTriggered(); }
        },
        {
            ":/Assets/icons/constraints/lineLineAngle.svg", "Angle Between Lines",
            [this] { emit angleBetweenLinesTriggered(); }
        }
    };

    for (const auto& [icon, tooltip, trigger]: constraints_) {
        auto* btn = reqsContainer_->addButton(QIcon(icon));
        btn->setFixedSize(buttonSize);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip(tooltip);
        btn->setIconSize(buttonSize);

        connect(btn, &QPushButton::clicked, this, trigger);
    }


    connect(linePrimitive_, &MenuButton::leftClickedTriggered,
            this, &ToolBar::handlePrimitiveActionSlot);

    connect(circlePrimitive_, &MenuButton::leftClickedTriggered,
            this, &ToolBar::handlePrimitiveActionSlot);

    connect(arcPrimitive_, &MenuButton::leftClickedTriggered,
            this, &ToolBar::handlePrimitiveActionSlot);


    connect(lineMenu_, &CustomMenu::actionSelected,
            this, &ToolBar::handlePrimitiveActionSlot);

    connect(circleMenu_, &CustomMenu::actionSelected,
            this, &ToolBar::handlePrimitiveActionSlot);

    connect(arcMenu_, &CustomMenu::actionSelected,
            this, &ToolBar::handlePrimitiveActionSlot);


    connect(pointPrimitive_, &QPushButton::clicked, this, &ToolBar::pointTriggered);

    connect(cursorTool_, &QPushButton::clicked, this, &ToolBar::cursorTriggered);
    connect(sizeTool_, &QPushButton::clicked, this, &ToolBar::sizeTriggered);
}
