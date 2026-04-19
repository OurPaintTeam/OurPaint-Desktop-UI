#include "ContainerWidget.h"

#include <QDrag>

#include "DraggleButton.h"

#include  <QHBoxLayout>
#include <QMimeData>
#include <QMouseEvent>

#include "MenuButton.h"

namespace {
    constexpr int K_BUTTON_SIZE_PX = 25;
    constexpr int K_CONTAINER_LAYOUT_SPACING_PX = 4;
} // namespace


UI::ContainerWidget::ContainerWidget(const ContainerType type,
                                     const Qt::Orientation orientation,
                                     QWidget* parent)
    : QWidget(parent)
      , type_(type)
      , orientation_(orientation) {
    setAttribute(Qt::WA_StyledBackground, true);


    rebuildLayout();
    setObjectName(QStringLiteral("ContainerWidget"));
}


UI::ContainerType UI::ContainerWidget::type() const {
    return type_;
}


Qt::Orientation UI::ContainerWidget::orientation() const {
    return orientation_;
}


void UI::ContainerWidget::setOrientation(Qt::Orientation o) {
    if (orientation_ == o) {
        return;
    }

    orientation_ = o;
    rebuildLayout();
}


QPushButton* UI::ContainerWidget::addButton(const QIcon& icon) {
    if (!layout_) {
        return nullptr;
    }

    auto* btn = new DraggleButton(icon, this);

    connect(btn,
            &DraggleButton::rightClicked,
            this,
            &ContainerWidget::startDrag);

    layout_->addWidget(btn, 0, Qt::AlignCenter);

    return btn;
}


QPushButton* UI::ContainerWidget::addButton(const QString& text) {
    if (!layout_) {
        return nullptr;
    }

    auto* btn = new DraggleButton(text, this);

    connect(btn,
            &DraggleButton::rightClicked,
            this,
            &ContainerWidget::startDrag);

    layout_->addWidget(btn, 0, Qt::AlignCenter);

    return btn;
}


UI::MenuButton* UI::ContainerWidget::addMenuButton(const QString& text,
                                                   QMenu* menu) {
    if (!layout_) {
        return nullptr;
    }

    auto* btn = new MenuButton(text, menu, this);

    btn->setFixedSize(K_BUTTON_SIZE_PX, K_BUTTON_SIZE_PX);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setDraw(false);

    btn->bindMenu(menu);
    btn->setInitialFromMenu();

    layout_->addWidget(btn, 0, Qt::AlignCenter);

    return btn;
}


void UI::ContainerWidget::startDrag() {
    setCursor(Qt::ClosedHandCursor);

    auto drag = std::make_unique<QDrag>(this);
    auto mime = std::make_unique<QMimeData>();

    mime->setData(
        "application/x-container",
        QByteArray::number(reinterpret_cast<quintptr>(this))
    );

    mime->setData(
        "application/x-window-id",
        QByteArray::number(reinterpret_cast<quintptr>(window()))
    );

    drag->setMimeData(mime.release());
    drag->exec(Qt::MoveAction);

    setCursor(Qt::ArrowCursor);
}


void UI::ContainerWidget::setCurrentSlot(SlotWidget* slot) {
    currentSlot_ = slot;
}


UI::SlotWidget* UI::ContainerWidget::currentSlot() const {
    return currentSlot_;
}


void UI::ContainerWidget::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        startDrag();
    }
}


void UI::ContainerWidget::rebuildLayout() {
    QList<QWidget*> widgets;

    if (layout_) {
        while (layout_->count()) {
            const auto* item = layout_->takeAt(0);

            if (item->widget()) {
                widgets << item->widget();
            }

            delete item;
        }

        delete layout_;
    }

    layout_ = (orientation_ == Qt::Horizontal)
                  ? static_cast<QBoxLayout*>(new QHBoxLayout(this))
                  : static_cast<QBoxLayout*>(new QVBoxLayout(this));

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(K_CONTAINER_LAYOUT_SPACING_PX);

    for (auto* w: widgets) {
        layout_->addWidget(w);
    }
}
