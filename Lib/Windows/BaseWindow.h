#ifndef OURPAINT_DESKTOP_UI_BASEWINDOW_H
#define OURPAINT_DESKTOP_UI_BASEWINDOW_H

#include "FramelessWindow.h"

namespace UI {
    class NotificationContainer;


    class BaseWindow : public FramelessWindow {
        Q_OBJECT

    public:
        explicit BaseWindow(QWidget* parent = nullptr);
        ~BaseWindow() override = default;

        void addNotification(const QString& text) const;

    protected:
        void initNotificationManager();

        NotificationContainer* notificationManager_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_BASEWINDOW_H
