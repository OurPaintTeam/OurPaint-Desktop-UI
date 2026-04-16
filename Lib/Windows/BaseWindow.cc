#include "BaseWindow.h"

#include "NotificationContainer.h"


class QOpenGLWindow;


UI::BaseWindow::BaseWindow(QWidget* parent)
    : FramelessWindow(parent) {
}


void UI::BaseWindow::addNotification(const QString& text) const {
    if (notificationManager_) {
        notificationManager_->addNotification(text);
    }
}


void UI::BaseWindow::initNotificationManager() {
    if (!notificationManager_) {
        notificationManager_ = new NotificationContainer(this);
    }
}
