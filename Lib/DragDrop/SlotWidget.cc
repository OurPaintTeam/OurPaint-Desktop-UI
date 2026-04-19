#include "SlotWidget.h"

#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QVBoxLayout>

#include "ContainerWidget.h"
#include "RotatedLabel.h"
#include "ToolsType.h"

namespace {
    constexpr int K_SLOT_MARGINS_PX = 4;
    constexpr int K_SLOT_SPACING_PX = 25;
    constexpr auto K_SLOT_PLACEHOLDER_TEXT = "Drop here";
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
    bool wasEmpty = containers_.isEmpty();

    removePlaceholder();

    c->setCurrentSlot(this);
    c->setParent(this);
    c->setOrientation(orientation_);

    const auto align =
            (orientation_ == Qt::Horizontal)
                ? (Qt::AlignLeft | Qt::AlignVCenter)
                : (Qt::AlignHCenter | Qt::AlignTop);

    if (wasEmpty) {
        addStretch();
        layout_->insertWidget(0, c, 0, align);
    } else {
        int insertIndex = layout_->count();
        if (stretchAdded_ && stretchWidget_) {
            insertIndex = layout_->indexOf(stretchWidget_);
        }
        layout_->insertWidget(insertIndex, c, 0, align);
    }

    containers_.append(c);
    updateStretchPosition();
}


void UI::SlotWidget::dragEnterEvent(QDragEnterEvent* e) {
    const auto* mime = e->mimeData();
    if (!mime){ return;}

    if (!mime->hasFormat("application/x-container"))
     {   return;}

    if (!mime->hasFormat("application/x-window-id"))
      {  return;}

    const auto windowId =
        mime->data("application/x-window-id").toULongLong();

    if (windowId != reinterpret_cast<quintptr>(this->window()))
      {  return;}

    const auto ptr =
        mime->data("application/x-container").toULongLong();

    auto* c = reinterpret_cast<ContainerWidget*>(ptr);

    if (!canAccept(c))
        {return;}

    e->acceptProposedAction();
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
    constexpr auto K_CONTAINER_MIME_TYPE = "application/x-container";
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
        removeStretch();
        createPlaceholder();
    }
}


void UI::SlotWidget::createPlaceholder() {
    if (placeholder_) {
        return;
    }

    placeholder_ = new UI::RotatedLabel(K_SLOT_PLACEHOLDER_TEXT, this);
    placeholder_->setAlignment(Qt::AlignCenter);
    placeholder_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

void UI::SlotWidget::addStretch() {
    if (stretchAdded_) {
        return;
    }

    stretchWidget_ = new QWidget(this);

    if (orientation_ == Qt::Horizontal) {
        stretchWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        stretchWidget_->setMinimumWidth(0);
    } else {
        stretchWidget_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        stretchWidget_->setMinimumHeight(0);
    }

    stretchWidget_->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    stretchWidget_->setAcceptDrops(true);
    stretchWidget_->installEventFilter(this);

     //stretchWidget_->setStyleSheet("background: rgba(100, 100, 255, 0.1);");

    layout_->addWidget(stretchWidget_);
    stretchAdded_ = true;
}

void UI::SlotWidget::removeStretch() {
    if (!stretchAdded_ || !stretchWidget_) {
        return;
    }

    layout_->removeWidget(stretchWidget_);
    delete stretchWidget_;
    stretchWidget_ = nullptr;
    stretchAdded_ = false;
}

void UI::SlotWidget::updateStretchPosition() {
    if (!stretchAdded_ || !stretchWidget_) {
        return;
    }

    // Перемещаем пружину в конец
    layout_->removeWidget(stretchWidget_);
    layout_->addWidget(stretchWidget_);
}

bool UI::SlotWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == stretchWidget_) {
        if (event->type() == QEvent::DragEnter) {
            auto* dragEvent = static_cast<QDragEnterEvent*>(event);
            auto* c = containerFromEvent(dragEvent);
            if (c && canAccept(c)) {
                dragEvent->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            auto* dropEvent = static_cast<QDropEvent*>(event);
            auto* c = containerFromEvent(dropEvent);
            if (c && canAccept(c)) {
                if (auto* oldSlot = qobject_cast<SlotWidget*>(c->parentWidget())) {
                    oldSlot->removeContainer(c);
                }
                insert(c);
                dropEvent->acceptProposedAction();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}