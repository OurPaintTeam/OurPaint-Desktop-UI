#include "DraggleContainer.h"

#include <QBoxLayout>
#include <QDrag>
#include <QLabel>
#include <QMimeData>

#include "MenuButton.h"
#include "TopBarTab.h"

/*           DoubleClickButton           */

namespace {
    constexpr int K_BUTTON_SIZE_PX = 25;
    constexpr int K_CONTAINER_LAYOUT_SPACING_PX = 4;
    constexpr int K_SLOT_MARGINS_PX = 4;
    constexpr int K_SLOT_SPACING_PX = 8;
    constexpr auto K_CONTAINER_MIME_TYPE = "application/x-container";
    constexpr auto K_SLOT_PLACEHOLDER_TEXT = "Drop here";
} // namespace

UI::DoubleClickButton::DoubleClickButton(const QString& text,
                                         QWidget* parent)
    : QPushButton(text, parent) {
    setFixedSize(K_BUTTON_SIZE_PX, K_BUTTON_SIZE_PX);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setObjectName(QStringLiteral("DoubleClickButton"));
}


void UI::DoubleClickButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        emit rightClicked();
    }

    QPushButton::mousePressEvent(e);
}


/*           ContainerWidget           */

UI::ContainerWidget::ContainerWidget(const ContainerType type,
                                     const Qt::Orientation orientation,
                                     QWidget* parent)
    : QWidget(parent)
      , type_(type)
      , orientation_(orientation) {
    setAttribute(Qt::WA_StyledBackground, true);
    setCursor(Qt::OpenHandCursor);

    rebuildLayout();
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


QPushButton* UI::ContainerWidget::addButton(const QString& text) {
    if (!layout_) {
        return nullptr;
    }

    auto* btn = new DoubleClickButton(text, this);

    connect(btn,
            &DoubleClickButton::rightClicked,
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
    btn->setLeftClick(false);

    btn->bindMenu(menu);
    btn->setInitialFromMenu();

    layout_->addWidget(btn, 0, Qt::AlignCenter);

    return btn;
}


void UI::ContainerWidget::startDrag() {
    setCursor(Qt::ClosedHandCursor);

    auto* drag = new QDrag(this);
    auto* mime = new QMimeData;

    mime->setData(
        K_CONTAINER_MIME_TYPE,
        QByteArray::number(reinterpret_cast<quintptr>(this))
    );

    drag->setMimeData(mime);
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
    if (e->button() == Qt::LeftButton) {
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


/*           SlotWidget           */

UI::SlotWidget::SlotWidget(const QSet<ContainerType>& acceptedTypes,
                           const Qt::Orientation orientation,
                           QWidget* parent)
    : QWidget(parent)
      , acceptedTypes_(acceptedTypes)
      , orientation_(orientation),layout_(
      (orientation == Qt::Horizontal)
                        ? static_cast<QBoxLayout*>(new QHBoxLayout(this))
                        : static_cast<QBoxLayout*>(new QVBoxLayout(this))
      ) {
    setAcceptDrops(true);

    layout_->setContentsMargins(K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX);
    layout_->setSpacing(K_SLOT_SPACING_PX);

    createPlaceholder();
}


void UI::SlotWidget::insert(ContainerWidget* c) {
    removePlaceholder();

    c->setCurrentSlot(this);
    c->setParent(this);
    c->setOrientation(orientation_);

    const auto align =
            (orientation_ == Qt::Horizontal)
                ? (Qt::AlignLeft | Qt::AlignVCenter)
                : (Qt::AlignHCenter | Qt::AlignTop);

    layout_->addWidget(c, 0, align);

    containers_.append(c);
}


void UI::SlotWidget::dragEnterEvent(QDragEnterEvent* e) {
    const auto* mime = e->mimeData();
    if (!mime || !mime->hasFormat(K_CONTAINER_MIME_TYPE)) {
        return;
    }

    if (const quintptr ptr = mime->data(K_CONTAINER_MIME_TYPE).toULongLong(); canAccept(reinterpret_cast<ContainerWidget*>(ptr))) {
        e->acceptProposedAction();
    }
}


void UI::SlotWidget::dropEvent(QDropEvent* e) {
    auto* c = containerFromEvent(e);

    if (!canAccept(c)) {
        return;
    }

    if (auto* oldSlot =
            qobject_cast<SlotWidget*>(c->parentWidget())) {
        oldSlot->removeContainer(c);
    }

    insert(c);

    if (c->type() == ContainerType::Side) {
        emit migrateContainer(c);
    }

    e->acceptProposedAction();
}


bool UI::SlotWidget::canAccept(const ContainerWidget* c) const {
    return c && acceptedTypes_.contains(c->type());
}


UI::ContainerWidget* UI::SlotWidget::containerFromEvent(const QDropEvent* e) {
    if (!e->mimeData()->hasFormat(K_CONTAINER_MIME_TYPE)) {
        return nullptr;
    }

    const quintptr ptr =
            e->mimeData()
            ->data(K_CONTAINER_MIME_TYPE)
            .toULongLong();

    return reinterpret_cast<ContainerWidget*>(ptr);
}


void UI::SlotWidget::removeContainer(ContainerWidget* c) {
    layout_->removeWidget(c);

    containers_.removeOne(c);

    c->setParent(nullptr);
    c->setCurrentSlot(nullptr);

    if (containers_.isEmpty()) {
        createPlaceholder();
    }
}


void UI::SlotWidget::createPlaceholder() {
    if (placeholder_) {
        return;
    }

    placeholder_ = new QLabel(K_SLOT_PLACEHOLDER_TEXT, this);
    placeholder_->setAlignment(Qt::AlignCenter);

    layout_->addWidget(placeholder_);
}


void UI::SlotWidget::removePlaceholder() {
    if (!placeholder_) {
        return;
    }

    layout_->removeWidget(placeholder_);

    delete placeholder_;
    placeholder_ = nullptr;
}
