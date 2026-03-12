#include "ToolBar.h"

#include <QStackedWidget>

#include "ArcMenu.h"
#include "CircleMenu.h"
#include "DraggleContainer.h"
#include "LineMenu.h"
#include "MenuButton.h"

namespace {
    const auto BUTTON_STYLE = R"(
    QPushButton {
        background: transparent;
        border: none;
        color: #D8D8F6;
        border-radius: 6px;
    }
    QPushButton:hover {
        background: rgba(255,255,255,0.08);
    }

    QPushButton:pressed {
        background: rgba(255,255,255,0.16);
    }
)";
} // namespace


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
      figuresSlot_(
          new SlotWidget(
              {ContainerType::Tools, ContainerType::Figures, ContainerType::Requirements},
              Qt::Horizontal
          )), figuresContainer_(new ContainerWidget(ContainerType::Figures, Qt::Horizontal)),
      reqsContainer_(new ContainerWidget(ContainerType::Requirements, Qt::Horizontal)) {
    setObjectName(QStringLiteral("toolbar"));
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background: transparent;");

    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    mainLayout_->addWidget(stack_);

    firstLayout_->setContentsMargins(0, 0, 0, 0);
    firstLayout_->setSpacing(0);
    constexpr auto spacing = 10;
    firstLayout_->addSpacing(spacing);

    figuresContainer_->addButton("P");
    figuresContainer_->addMenuButton("", arcMenu_);
    figuresContainer_->addMenuButton("", circleMenu_);
    figuresContainer_->addMenuButton("", lineMenu_);

    constexpr auto toolsSize = QSize(25, 25);
    for (const QString& text: {"M", "S", "S"}) {
        auto* b = toolsContainer_->addButton(text);
        b->setFixedSize(toolsSize);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(BUTTON_STYLE);
    }

    firstLayout_->addWidget(figuresSlot_);
    figuresSlot_->insert(figuresContainer_);
    figuresSlot_->insert(toolsContainer_);

    stack_->addWidget(firstLaneWidget_);

    secondLayout_->setContentsMargins(0, 0, 0, 0);
    secondLayout_->setSpacing(0);
    secondLayout_->addSpacing(spacing);

    reqsSlot_ = new SlotWidget(
        {ContainerType::Tools, ContainerType::Figures, ContainerType::Requirements},
        Qt::Horizontal
    );

    constexpr auto index = 10;
    for (int i = 1; i <= index; ++i) {
        auto* b = reqsContainer_->addButton(QString::number(i));
        constexpr auto buttonSize = QSize(25, 25);
        b->setFixedSize(buttonSize);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(BUTTON_STYLE);
    }

    secondLayout_->addWidget(reqsSlot_);
    reqsSlot_->insert(reqsContainer_);

    stack_->addWidget(secondLaneWidget_);

    stack_->setCurrentIndex(0);
    isFirstLaneVisible_ = true;

    figuresSlot_->setObjectName("sideSlot");
    figuresSlot_->setStyleSheet(R"(
            QWidget#sideSlot {
                border: 1px dashed #666;
            }
        )");

    reqsSlot_->setObjectName("sideSlot");
    reqsSlot_->setStyleSheet(R"(
            QWidget#sideSlot {
                border: 1px dashed #666;
            }
        )");
}


void UI::ToolBar::wheelEvent(QWheelEvent* event) {
    const int pageCount = stack_->count();
    int current = stack_->currentIndex();

    if (event->angleDelta().y() > 0) {
        current = (current - 1 + pageCount) % pageCount;
    } else {
        current = (current + 1) % pageCount;
    }

    stack_->setCurrentIndex(current);
    isFirstLaneVisible_ = (current == 0);

    event->accept();
}
