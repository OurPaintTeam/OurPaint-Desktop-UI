#include "NotificationContainer.h"

#include <QEvent>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QPainter>
#include <QScrollBar>

#include "NotificationWidget.h"


UI::NotificationContainer::NotificationContainer(QWidget *parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this)),
      containerWidget_(new QWidget(this)),
      scrollArea_(new QScrollArea(this)),
      hideTimer_(new QTimer(this)) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
  setFixedWidth(320);
    containerLayout_ = new QVBoxLayout(containerWidget_);
    containerLayout_->setAlignment(Qt::AlignTop);
    containerWidget_->setLayout(containerLayout_);

    scrollArea_->setWidget(containerWidget_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setObjectName("ScrollNotification");
    scrollArea_->viewport()->setObjectName("ScrollNotificationViewport");
    scrollArea_->verticalScrollBar()->setObjectName("VertScrollNotification");
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->verticalScrollBar()->setVisible(false);
    scrollArea_->verticalScrollBar()->setFixedWidth(0);
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

    const auto *const p = parentWidget();

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
    auto *const widget = new NotificationWidget(text, containerWidget_);
    connect(widget, &NotificationWidget::deleted,
            this, &NotificationContainer::removeNotification);

    auto *const containerLayout =
            qobject_cast<QVBoxLayout*>(containerWidget_->layout());

    containerLayout->addWidget(widget);

    notifications_.prepend(widget);

    updateContainerSize();

    if (!isVisible()) {
        show();
    }

    QTimer::singleShot(0, this, [this]() {
        auto *bar = scrollArea_->verticalScrollBar();
        bar->setValue(bar->maximum());
    });

    startHideTimer();
}


void UI::NotificationContainer::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    hideTimer_->stop();
}


void UI::NotificationContainer::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    if (!notifications_.isEmpty()) {
        startHideTimer();
    }
}


bool UI::NotificationContainer::eventFilter(QObject *obj, QEvent *event) {
    if (obj == parentWidget() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
        updatePosition();
    }

    return QWidget::eventFilter(obj, event);
}


void UI::NotificationContainer::removeNotification(NotificationWidget *widget) {
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
  int totalHeight = 0;

  for (auto *w : notifications_) {
    totalHeight += w->sizeHint().height();
  }

  const int maxVisibleHeight = 240;

  if (totalHeight <= maxVisibleHeight) {
    containerWidget_->setFixedHeight(totalHeight);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedHeight(totalHeight);
  } else {
    containerWidget_->setFixedHeight(totalHeight);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFixedHeight(maxVisibleHeight);
  }

  updatePosition();
}
