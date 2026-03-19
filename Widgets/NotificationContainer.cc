#include "NotificationContainer.h"

#include <QEvent>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

#include "NotificationWidget.h"


UI::NotificationContainer::NotificationContainer(QWidget* parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this)),
      containerWidget_(new QWidget(this)), scrollArea_(new QScrollArea(this)),
      hideTimer_(new QTimer(this)) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    layout_->setContentsMargins(0, 0, 0, 0);
    containerLayout_->setAlignment(Qt::AlignTop);
    containerLayout_->setContentsMargins(0, 0, 0, 0);
    containerWidget_->setLayout(containerLayout_);

    scrollArea_->setWidget(containerWidget_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout_->addWidget(scrollArea_);

    constexpr auto x = 300;
    setFixedWidth(x);

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

    const auto globalBottomRight = p->mapToGlobal(p->rect().bottomRight());

    move(globalBottomRight.x() - width() - margin,
         globalBottomRight.y() - height() - margin);

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
    connect(widget, &NotificationWidget::deleted,
            this, &NotificationContainer::removeNotification);

    auto* const containerLayout =
            qobject_cast<QVBoxLayout*>(containerWidget_->layout());

    containerLayout->addWidget(widget);
    notifications_.append(widget);

    updateContainerSize();

    if (!isVisible()) {
        show();
    }

    startHideTimer();
}


void UI::NotificationContainer::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    hideTimer_->stop();
}


void UI::NotificationContainer::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    if (!notifications_.isEmpty()) {
        startHideTimer();
    }
}


bool UI::NotificationContainer::eventFilter(QObject* obj, QEvent* event) {
    if (obj == parentWidget() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
        updatePosition();
    }

    return QWidget::eventFilter(obj, event);
}


void UI::NotificationContainer::removeNotification(NotificationWidget* widget) {
    if (widget && notifications_.contains(widget)) {
        notifications_.removeOne(widget);
        widget->deleteLater();
        updateContainerSize();
    }

    if (notifications_.isEmpty()) {
        hide();
        hideTimer_->stop();
    }
}


void UI::NotificationContainer::updateContainerSize() {
    const qsizetype count = notifications_.size();
    constexpr int widgetHeight = 50;

    if (count <= 3) {
        containerWidget_->setFixedHeight(count * widgetHeight);
        scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFixedHeight(count * widgetHeight + 4);
    } else {
        containerWidget_->setFixedHeight(count * widgetHeight);
        scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setFixedHeight(3 * widgetHeight + 4);
    }

    updatePosition();
}
