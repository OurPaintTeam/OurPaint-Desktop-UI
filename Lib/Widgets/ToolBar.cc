#include "ToolBar.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QStyle>
#include <QWheelEvent>

#include "ArcMenu.h"
#include "CircleMenu.h"
#include "ContainerWidget.h"
#include "LineMenu.h"
#include "MenuButton.h"
#include "SlotWidget.h"
#include "ToolsType.h"


UI::ToolBar::ToolBar(QWidget *parent)
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
      reqsContainer_(new ContainerWidget(ContainerType::Tools, Qt::Horizontal)) {
    setObjectName("ToolBar");
    constexpr auto h = 32;
    setFixedHeight(h);
    setAttribute(Qt::WA_StyledBackground, true);

    initLayout();
    initFiguresLane();
    initConstraintsLane();
    initSignals();
    translate();

    stack_->setCurrentIndex(0);
}


// VISIBILITY
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


// EVENTS
void UI::ToolBar::wheelEvent(QWheelEvent *event) {
    event->accept();
}


void UI::ToolBar::changeEvent(QEvent *e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}


// INIT
void UI::ToolBar::initLayout() const {
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    mainLayout_->addWidget(stack_);

    firstLaneWidget_->setAttribute(Qt::WA_StyledBackground, true);
    secondLaneWidget_->setAttribute(Qt::WA_StyledBackground, true);
}


// FIGURES
void UI::ToolBar::initFiguresLane() {
    constexpr int spacing = 10;

    firstLayout_->setContentsMargins(0, 0, 0, 0);
    firstLayout_->setSpacing(0);
    firstLayout_->addSpacing(spacing);

    // === PRIMITIVES ===
    pointPrimitive_ = figuresContainer_->addButton(QIcon(":/Assets/icons/primitives/point.svg"));
    pointPrimitive_->setObjectName("NoActiveTool");

    linePrimitive_ = figuresContainer_->addMenuButton("", lineMenu_);
    circlePrimitive_ = figuresContainer_->addMenuButton("", circleMenu_);
    arcPrimitive_ = figuresContainer_->addMenuButton("", arcMenu_);

    for (auto *btn: {linePrimitive_, circlePrimitive_, arcPrimitive_}) {
        btn->setOpenLeftClick(false);
        btn->setChange(true);
        btn->setObjectName("NoActiveTool");
    }

    cubicBezierPrimitive_ = figuresContainer_->addButton(QIcon(":/Assets/icons/primitives/beze.png"));
    cubicBezierPrimitive_->setObjectName("NoActiveTool");

    // TOOLS
    constexpr QSize size(25, 25);

    cursorTool_ = toolsContainer_->addButton(QIcon(":/Assets/icons/tools/cursorTool.png"));
    cursorTool_->setFixedSize(size);
    cursorTool_->setCursor(Qt::PointingHandCursor);

    sizeTool_ = toolsContainer_->addButton(QIcon(":/Assets/icons/tools/sizeTool.svg"));
    sizeTool_->setFixedSize(size);
    sizeTool_->setCursor(Qt::PointingHandCursor);

    activeTool_ = cursorTool_;
    activeTool_->setObjectName("ActiveTool");

    // LAYOUT
    firstLayout_->addWidget(figuresSlot_);
    firstLayout_->addSpacing(spacing);

    figuresSlot_->insert(toolsContainer_);
    figuresSlot_->insert(figuresContainer_);

    stack_->addWidget(firstLaneWidget_);
}


// CONSTRAINTS
void UI::ToolBar::initConstraintsLane() {
    secondLayout_->setContentsMargins(0, 0, 0, 0);
    secondLayout_->setSpacing(0);

    reqsSlot_ = new SlotWidget({ContainerType::Tools}, Qt::Horizontal);
    figuresSlot_->insert(reqsContainer_);

    stack_->addWidget(secondLaneWidget_);
}


// SIGNALS
void UI::ToolBar::initSignals() {
    constexpr QSize buttonSize(25, 25);

    // CONSTRAINTS
    constraints_ = {
        {":/Assets/icons/constraints/Coincident.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/HorizontalVertical.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Parallel.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Perpendicular.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Tangent.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Equal.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/FixUnfix.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Concentric.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Collinear.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Midpoint.svg", new QAction("", this), nullptr},
        {":/Assets/icons/constraints/Symmetric.svg", new QAction("", this), nullptr}
    };

    for (auto& item: constraints_) {
        auto *btn = reqsContainer_->addButton(QIcon(item.icon));
        btn->setFixedSize(buttonSize);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("NoActiveTool");

        item.button = btn;

        connect(btn, &QPushButton::clicked, this, [this, act = item.action]() {
            act->trigger();
        });
    }

    // PRIMITIVES
    connect(pointPrimitive_, &QPushButton::clicked, this, [this]() {
        emit primitiveTriggered(PrimitiveType::Point);
        setActiveTool(pointPrimitive_);
    });

    connect(cubicBezierPrimitive_, &QPushButton::clicked, this, [this]() {
        emit primitiveTriggered(PrimitiveType::CubicBezier);
        setActiveTool(cubicBezierPrimitive_);
    });

    // MENU BUTTONS
    auto connectMenu = [this](MenuButton *btn) {
        connect(btn, &MenuButton::leftClickedTriggered, this,
                [this, btn](const QAction *a) {
                    handlePrimitiveActionSlot(a);
                    setActiveTool(btn);
                });
    };

    connectMenu(linePrimitive_);
    connectMenu(circlePrimitive_);
    connectMenu(arcPrimitive_);

    // MENU SIGNALS
    auto connectMenuSignal = [this](const CustomMenu *menu, MenuButton *btn) {
        connect(menu, &CustomMenu::actionSelected, this,
                [this, btn](const QAction *a) {
                    handlePrimitiveActionSlot(a);
                    setActiveTool(btn);
                });
    };

    connectMenuSignal(lineMenu_, linePrimitive_);
    connectMenuSignal(circleMenu_, circlePrimitive_);
    connectMenuSignal(arcMenu_, arcPrimitive_);

    // TOOLS
    connect(cursorTool_, &QPushButton::clicked, this, [this]() {
        emit toolsTriggered(ToolsType::Cursor);
        setActiveTool(cursorTool_);
    });

    connect(sizeTool_, &QPushButton::clicked, this, [this]() {
        emit toolsTriggered(ToolsType::Size);
        setActiveTool(sizeTool_);
    });
}


// LOGIC
void UI::ToolBar::handlePrimitiveActionSlot(const QAction *action) {
    if (!action) { return; }

    const auto type = action->data().value<PrimitiveType>();
    emit primitiveTriggered(type);
}


// ACTIVE TOOL
void UI::ToolBar::setActiveTool(const ToolsType tool) {
    switch (tool) {
        case ToolsType::Cursor: setActiveTool(cursorTool_);
            break;
        case ToolsType::Size: setActiveTool(sizeTool_);
            break;
        default: break;
    }
}


void UI::ToolBar::setActiveTool(const PrimitiveType tool) {
    switch (tool) {
        case PrimitiveType::Point: setActiveTool(pointPrimitive_);
            break;
        case PrimitiveType::CubicBezier: setActiveTool(cubicBezierPrimitive_);
            break;

        case PrimitiveType::Line:
            linePrimitive_->applyAction(lineMenu_->line());
            setActiveTool(linePrimitive_);
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


// TRANSLATE
void UI::ToolBar::translate() const {
    if (pointPrimitive_) {
        pointPrimitive_->setToolTip(UI::ToolBar::tr("Point"));
    }
    if (cubicBezierPrimitive_) {
        cubicBezierPrimitive_->setToolTip(UI::ToolBar::tr("CubicBezier"));
    }
    if (cursorTool_) {
        cursorTool_->setToolTip(UI::ToolBar::tr("Cursor tool"));
    }
    if (sizeTool_) {
        sizeTool_->setToolTip(UI::ToolBar::tr("Show size"));
    }

    // CONSTRAINTS
    if (constraints_.size() >= 11) {
        constraints_[0].button->setToolTip(UI::ToolBar::tr("Coincident"));
        constraints_[1].button->setToolTip(UI::ToolBar::tr("HorizontalVertical"));
        constraints_[2].button->setToolTip(UI::ToolBar::tr("Parallel"));
        constraints_[3].button->setToolTip(UI::ToolBar::tr("Perpendicular"));
        constraints_[4].button->setToolTip(UI::ToolBar::tr("Tangent"));
        constraints_[5].button->setToolTip(UI::ToolBar::tr("Equal"));
        constraints_[6].button->setToolTip(UI::ToolBar::tr("FixUnfix"));
        constraints_[7].button->setToolTip(UI::ToolBar::tr("Concentric"));
        constraints_[8].button->setToolTip(UI::ToolBar::tr("Collinear"));
        constraints_[9].button->setToolTip(UI::ToolBar::tr("Midpoint"));
        constraints_[10].button->setToolTip(UI::ToolBar::tr("Symmetric"));
    }
}
