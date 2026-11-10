#include "BaseWindow.h"
#include "NotificationManager.h"

UI::BaseWindow::BaseWindow(QWidget* parent)
    : FramelessWindow(parent) {
    if (!notificationManager_) {
        notificationManager_ = new NotificationManager(const_cast<BaseWindow*>(this), this);
    }
}

void UI::BaseWindow::addNotification(const QString& text) const {
    if (notificationManager_) {
        notificationManager_->addNotification(text);
    }
}

void UI::BaseWindow::includeNotificationManager(const bool enabled) {
    if (notificationManager_) {
        notificationManager_->setEnabled(enabled);
    }
}