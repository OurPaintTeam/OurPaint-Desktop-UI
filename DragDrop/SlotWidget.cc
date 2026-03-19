#include "SlotWidget.h"

#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>

#include "ContainerWidget.h"
#include "RotatedLabel.h"
#include "ToolsType.h"

namespace {
    constexpr int K_SLOT_MARGINS_PX = 4;
    constexpr int K_SLOT_SPACING_PX = 8;
    constexpr auto K_SLOT_PLACEHOLDER_TEXT = "Drop here";
    constexpr auto K_CONTAINER_MIME_TYPE = "application/x-container";
} // namespace

UI::SlotWidget::SlotWidget(const QSet<ContainerType>& acceptedTypes,
                           const Qt::Orientation orientation,
                           QWidget* parent)
    : QWidget(parent)
      , acceptedTypes_(acceptedTypes)
      , orientation_(orientation), layout_(
          (orientation == Qt::Horizontal)
              ? static_cast<QBoxLayout*>(new QHBoxLayout(this))
              : static_cast<QBoxLayout*>(new QVBoxLayout(this))
      ) {
    setAcceptDrops(true);

    layout_->setContentsMargins(K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX, K_SLOT_MARGINS_PX);
    layout_->setSpacing(K_SLOT_SPACING_PX);

    createPlaceholder();
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QStringLiteral("SlotWidget"));
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

    if (const quintptr ptr = mime->data(K_CONTAINER_MIME_TYPE).toULongLong(); canAccept(
        reinterpret_cast<ContainerWidget*>(ptr))) {
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

    placeholder_ = new UI::RotatedLabel(K_SLOT_PLACEHOLDER_TEXT, this);
    placeholder_->setAlignment(Qt::AlignCenter);
    if (orientation_ == Qt::Vertical) {
        constexpr int verticalAngle = 90;
        placeholder_->setAngle(verticalAngle);
    }

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
