#include "NotificationManager.h"
#include "NotificationWidget.h"
#include <QTimer>

UI::NotificationManager::NotificationManager(QWidget* anchorWidget, QObject* parent)
    : QObject(parent), anchorWidget_(anchorWidget) {
}

void UI::NotificationManager::addNotification(const QString& text) {
    if (!enabled_) {
        return;
    }
    queue_.enqueue(text);
    if (!isShowing_) {
        showNext();
    }
}


void UI::NotificationManager::setEnabled(bool enabled) {
    enabled_ = enabled;

    if (!enabled_) {
        queue_.clear();
        if (currentWidget_) {
            currentWidget_->close();
            currentWidget_ = nullptr;
        }
        isShowing_ = false;
    }
}


void UI::NotificationManager::showNext() {
    if (!enabled_ || queue_.isEmpty()) {
        isShowing_ = false;
        currentWidget_ = nullptr;
        return;
    }

    isShowing_ = true;
    const QString text = queue_.dequeue();

    currentWidget_ = new NotificationWidget(text, anchorWidget_, nullptr);

    connect(currentWidget_, &NotificationWidget::finished, this, [this]() {
        currentWidget_ = nullptr;

        QTimer::singleShot(200, this, &NotificationManager::showNext);
    });


    currentWidget_->showFor(3000);
}
