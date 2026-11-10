#ifndef OURPAINT_DESKTOP_UI_NOTIFICATIONMANAGER_H
#define OURPAINT_DESKTOP_UI_NOTIFICATIONMANAGER_H

#include <QObject>
#include <QQueue>
#include <QString>

namespace UI {
    class NotificationWidget;


    class NotificationManager final : public QObject {
        Q_OBJECT

    public:
        explicit NotificationManager(QWidget *anchorWidget, QObject *parent = nullptr);
        ~NotificationManager() override = default;

        void addNotification(const QString& text);

        void setEnabled(bool enabled);

    private slots:
        void showNext();
    private:
        QWidget *anchorWidget_{nullptr};
        QQueue<QString> queue_;
        NotificationWidget *currentWidget_{nullptr};
        bool isShowing_{false};
        bool enabled_{true};
    };
}
#endif
