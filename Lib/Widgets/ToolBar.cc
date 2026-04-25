#include "ToolBar.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QStyleOptionButton>
#include <QWheelEvent>

#include "ArcMenu.h"
#include "CircleMenu.h"
#include "ContainerWidget.h"
#include "LineMenu.h"
#include "MenuButton.h"
#include "SlotWidget.h"
#include "ToolsType.h"

UI::ToolBar::ToolBar(QWidget *parent)
    : QWidget(parent), arcMenu_(new ArcMenu(this)),
      circleMenu_(new CircleMenu(this)), lineMenu_(new LineMenu(this)),
      stack_(new QStackedWidget(this)), mainLayout_(new QHBoxLayout(this)),
      firstLaneWidget_(new QWidget(this)),
      firstLayout_(new QHBoxLayout(firstLaneWidget_)),
      secondLaneWidget_(new QWidget(this)),
      secondLayout_(new QHBoxLayout(secondLaneWidget_)),
      toolsContainer_(
          new ContainerWidget(ContainerType::Tools, Qt::Horizontal)),
      figuresSlot_(new SlotWidget({ContainerType::Tools}, Qt::Horizontal)),
      figuresContainer_(
          new ContainerWidget(ContainerType::Tools, Qt::Horizontal)),
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

void UI::ToolBar::wheelEvent(QWheelEvent *event) { event->accept(); }

void UI::ToolBar::handlePrimitiveActionSlot(const QAction *action) {
  if (!action) {
    return;
  }

  auto type = action->data().value<PrimitiveType>();
  emit primitiveTriggered(type);
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

  pointPrimitive_ = figuresContainer_->addButton(
      QIcon(":/Assets/icons/primitives/point.svg"));
  pointPrimitive_->setToolTip("Point");
  pointPrimitive_->setObjectName("NoActiveTool");
  pointPrimitive_->setText(QString());

  linePrimitive_ = figuresContainer_->addMenuButton("", lineMenu_);
  circlePrimitive_ = figuresContainer_->addMenuButton("", circleMenu_);
  arcPrimitive_ = figuresContainer_->addMenuButton("", arcMenu_);

  linePrimitive_->setOpenLeftClick(false);
  linePrimitive_->setChange(true);
  linePrimitive_->setObjectName("NoActiveTool");

  circlePrimitive_->setOpenLeftClick(false);
  circlePrimitive_->setChange(true);
  circlePrimitive_->setObjectName("NoActiveTool");

  arcPrimitive_->setOpenLeftClick(false);
  arcPrimitive_->setChange(true);
  arcPrimitive_->setObjectName("NoActiveTool");

  cubicBezierPrimitive_ = figuresContainer_->addButton(
      QIcon(":/Assets/icons/primitives/beze.png"));
  cubicBezierPrimitive_->setToolTip("CubicBezier");
  cubicBezierPrimitive_->setObjectName("NoActiveTool");

  constexpr QSize toolsSize(25, 25);
  cursorTool_ =
      toolsContainer_->addButton(QIcon(":/Assets/icons/tools/cursorTool.png"));
  cursorTool_->setFixedSize(toolsSize);
  cursorTool_->setCursor(Qt::PointingHandCursor);
  cursorTool_->setToolTip("Cursor tool");
  activeTool_ = cursorTool_;
  activeTool_->setObjectName("ActiveTool");

  sizeTool_ =
      toolsContainer_->addButton(QIcon(":/Assets/icons/tools/sizeTool.svg"));
  sizeTool_->setFixedSize(toolsSize);
  sizeTool_->setCursor(Qt::PointingHandCursor);
  sizeTool_->setToolTip("Show size");
  sizeTool_->setObjectName("NoActiveTool");

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
      {":/Assets/icons/constraints/pointLineDist.svg", "Point – Line Distance",
       [this] {
         auto type = ConstraintType::PointLineDistance;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/pointOnLine.svg", "Point on Line",
       [this] {
         auto type = ConstraintType::PointOnLine;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/pointPointDist.svg",
       "Point – Point Distance",
       [this] {
         auto type = ConstraintType::PointPointDistance;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/pointOnPoint.svg", "Coincident Points",
       [this] {
         auto type = ConstraintType::CoincidentPoints;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineCircleDist.svg",
       "Line – Circle Distance",
       [this] {
         auto type = ConstraintType::LineCircleDistance;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineOnCircle.svg", "Line on Circle",
       [this] {
         auto type = ConstraintType::LineOnCircle;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineInCircle.svg", "Line Inside Circle",
       [this] {
         auto type = ConstraintType::LineInCircle;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineLineParallel.svg", "Parallel Lines",
       [this] {
         auto type = ConstraintType::ParallelLines;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineLinePerpendicular.svg",
       "Perpendicular Lines",
       [this] {
         auto type = ConstraintType::PerpendicularLines;
         emit constraintTriggered(type);
       }},
      {":/Assets/icons/constraints/lineLineAngle.svg", "Angle Between Lines",
       [this] {
         auto type = ConstraintType::AngleBetweenLines;
         emit constraintTriggered(type);
       }}};

  for (const auto &[icon, tooltip, trigger] : constraints_) {
    auto *btn = reqsContainer_->addButton(QIcon(icon));
    btn->setFixedSize(buttonSize);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setToolTip(tooltip);
    btn->setObjectName("NoActiveTool");
    connect(btn, &QPushButton::clicked, this, trigger);
  }

  connect(linePrimitive_, &MenuButton::leftClickedTriggered, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(linePrimitive_);
          });

  connect(circlePrimitive_, &MenuButton::leftClickedTriggered, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(circlePrimitive_);
          });

  connect(arcPrimitive_, &MenuButton::leftClickedTriggered, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(arcPrimitive_);
          });

  connect(lineMenu_, &CustomMenu::actionSelected, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(linePrimitive_);
          });

  connect(circleMenu_, &CustomMenu::actionSelected, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(circlePrimitive_);
          });

  connect(arcMenu_, &CustomMenu::actionSelected, this,
          [this](const QAction *selection) {
            emit handlePrimitiveActionSlot(selection);
            setActiveTool(arcPrimitive_);
          });

  connect(pointPrimitive_, &QPushButton::clicked, this, [this]() {
    auto type = PrimitiveType::Point;
    emit primitiveTriggered(type);
    setActiveTool(pointPrimitive_);
  });

  connect(cubicBezierPrimitive_, &QPushButton::clicked, this, [this]() {
    auto type = PrimitiveType::CubicBezier;
    emit primitiveTriggered(type);
    setActiveTool(cubicBezierPrimitive_);
  });

  connect(cursorTool_, &QPushButton::clicked, this, [this]() {
    auto type = ToolsType::Cursor;
    emit toolsTriggered(type);
    setActiveTool(cursorTool_);
  });

  connect(sizeTool_, &QPushButton::clicked, this, [this]() {
    auto type = ToolsType::Size;
    emit toolsTriggered(type);
    setActiveTool(sizeTool_);
  });
}

void UI::ToolBar::setActiveTool(ToolsType tool) {
  switch (tool) {
  case ToolsType::Cursor:
    setActiveTool(cursorTool_);
    break;

  case ToolsType::Size:
    setActiveTool(sizeTool_);
    break;

  default:
    break;
  }
}

void UI::ToolBar::setActiveTool(PrimitiveType tool) {
    switch (tool) {

        case PrimitiveType::Point:
            setActiveTool(pointPrimitive_);
            break;

        case PrimitiveType::CubicBezier:
            setActiveTool(cubicBezierPrimitive_);
            break;

        case PrimitiveType::Line:
            linePrimitive_->applyAction(lineMenu_->line());
            setActiveTool(linePrimitive_);
            break;
        case PrimitiveType::LineSettings:
            linePrimitive_->applyAction(lineMenu_->settingsLine());
            setActiveTool(linePrimitive_);
        case PrimitiveType::Polyline:
            linePrimitive_->applyAction(lineMenu_->polyLine());
            setActiveTool(linePrimitive_);
        case PrimitiveType::InfiniteLine:
            linePrimitive_->applyAction(lineMenu_->infiniteLine());
            setActiveTool(linePrimitive_);
            break;


        case PrimitiveType::CircleByRadius:
            circlePrimitive_->applyAction(circleMenu_->circleByRadius());
            setActiveTool(circlePrimitive_);
            break;
          case PrimitiveType::CircleByDiameter:
            circlePrimitive_->applyAction(circleMenu_->circleByDiameter());
            setActiveTool(circlePrimitive_);
        case PrimitiveType::CircleByTwoPoints:
            circlePrimitive_->applyAction(circleMenu_->circleByTwoPoints());
            setActiveTool(circlePrimitive_);
            break;
        case PrimitiveType::CircleByThreePoints:
            circlePrimitive_->applyAction(circleMenu_->circleByThreePoints());
            setActiveTool(circlePrimitive_);
            break;
        case PrimitiveType::CircleTangentTwoLines:
            circlePrimitive_->applyAction(circleMenu_->circleTangentTwoLines());
            setActiveTool(circlePrimitive_);
            break;
        case PrimitiveType::CircleTangentThreeLines:
            circlePrimitive_->applyAction(circleMenu_->circleTangentThreeLines());
            setActiveTool(circlePrimitive_);
            break;


        case PrimitiveType::ArcByRadius:
            arcPrimitive_->applyAction(arcMenu_->arcRadius());
            setActiveTool(arcPrimitive_);
            break;
        case PrimitiveType::ArcByDiameter:
            arcPrimitive_->applyAction(arcMenu_->arcRadius());
            setActiveTool(arcPrimitive_);
            break;
        case PrimitiveType::ArcThreePoints:
            arcPrimitive_->applyAction(arcMenu_->arc3Points());
            setActiveTool(arcPrimitive_);
            break;
        case PrimitiveType::ArcSettings:
            arcPrimitive_->applyAction(arcMenu_->arc3Points());
            setActiveTool(arcPrimitive_);
            break;

        default:
            break;
    }
}

void UI::ToolBar::setActiveTool(QPushButton *tool) {
  if (activeTool_ == tool) {
    return;
  }

  if (activeTool_) {
    activeTool_->setObjectName("NoActiveTool");
    activeTool_->style()->unpolish(activeTool_);
    activeTool_->style()->polish(activeTool_);
  }

  activeTool_ = tool;
  tool->setObjectName("ActiveTool");

  tool->style()->unpolish(tool);
  tool->style()->polish(tool);
  tool->update();

  update();
}
