#include "NotificationContainer.h"

#include <QEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "NotificationWidget.h"

UI::NotificationContainer::NotificationContainer(QWidget* parent)
    : QWidget(parent), layout_(new QVBoxLayout(this)), containerWidget_(new QWidget(this)), scrollArea_(new QScrollArea(this)), hideTimer_(new QTimer(this)) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setFixedWidth(320);
    layout_->setContentsMargins(0, 0, 0, 0);
    containerLayout_ = new QVBoxLayout(containerWidget_);
    containerLayout_->setContentsMargins(8, 8, 8, 8);
    containerLayout_->setSpacing(6);
    containerLayout_->setAlignment(Qt::AlignTop);

    scrollArea_->setWidget(containerWidget_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setObjectName("ScrollNotification");
    scrollArea_->viewport()->setObjectName("ScrollNotificationViewport");
    scrollArea_->verticalScrollBar()->setObjectName("VertScrollNotification");
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout_->addWidget(scrollArea_);

    containerWidget_->setAttribute(Qt::WA_StyledBackground, true);
    containerWidget_->setAttribute(Qt::WA_TranslucentBackground, true);
    containerWidget_->setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    setObjectName("NotificationContainer");
    containerWidget_->setObjectName("NotificationContainerWidget");



    if (parent) {
        parent->installEventFilter(this);
    }

    hideTimer_->setSingleShot(true);
    connect(hideTimer_, &QTimer::timeout, this, &NotificationContainer::onHideTimeout);

    updateContainerSize();
    updatePosition();
    hide();
}

void UI::NotificationContainer::updatePosition() {
    if (!parentWidget()) {
        return;
    }

    constexpr int margin = 10;

    const auto* const p = parentWidget();

    const auto bottomRight = p->mapToGlobal(p->rect().bottomRight());
    move(bottomRight.x() - width() - margin, bottomRight.y() - height() - margin);

    raise();
}

void UI::NotificationContainer::startHideTimer() const {
    if (underMouse()) {
        hideTimer_->stop();
        return;
    }

    constexpr auto t = 3000;
    hideTimer_->start(t);
}

void UI::NotificationContainer::onHideTimeout() {
    if (!underMouse()) {
        hide();
    }
}

void UI::NotificationContainer::addNotification(const QString& text) {
    auto* const widget = new NotificationWidget(text, containerWidget_);
    connect(widget, &NotificationWidget::deleted, this, &NotificationContainer::removeNotification);

    containerLayout_->insertWidget(0, widget);

    notifications_.prepend(widget);
    widget->show();

    updateContainerSize();

    if (!isVisible()) {
        show();
    }

    QTimer::singleShot(0, this, [this]() {
        auto* bar = scrollArea_->verticalScrollBar();
        bar->setValue(bar->minimum());
    });

    startHideTimer();
}

void UI::NotificationContainer::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    hideTimer_->stop();
}

void UI::NotificationContainer::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    if (!notifications_.isEmpty()) {
        startHideTimer();
    }
}

bool UI::NotificationContainer::eventFilter(QObject* obj, QEvent* event) {
    if (obj == parentWidget()) {
        if (event->type() == QEvent::Resize) {
            updateContainerSize();
        } else if (event->type() == QEvent::Move) {
            updatePosition();
        }
    }

    return QWidget::eventFilter(obj, event);
}

void UI::NotificationContainer::removeNotification(NotificationWidget* widget) {
    if (widget && notifications_.contains(widget)) {
        notifications_.removeOne(widget);
        containerLayout_->removeWidget(widget);
        widget->hide();
        widget->deleteLater();
        updateContainerSize();
    }

    if (notifications_.isEmpty()) {
        hide();
        hideTimer_->stop();
    }
}

void UI::NotificationContainer::updateContainerSize() {
    constexpr int margin = 10;
    constexpr int maxVisibleHeight = 240;
    const auto* const parent = parentWidget();
    setFixedWidth(parent ? qMin(320, qMax(1, parent->width() - 2 * margin)) : 320);
    const int availableHeight = parent ? qMin(maxVisibleHeight, qMax(1, parent->height() - 2 * margin)) : maxVisibleHeight;

    // Measure wrapped text at the viewport width, including layout margins and spacing.
    containerLayout_->invalidate();
    const int totalHeight = containerLayout_->hasHeightForWidth() ? containerLayout_->totalHeightForWidth(width()) : containerLayout_->sizeHint().height();
    setFixedHeight(qMin(totalHeight, availableHeight));
    layout_->activate();
    containerLayout_->activate();

    updatePosition();
}
