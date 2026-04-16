#ifndef OURPAINT_DESKTOP_UI_NOTIFICATIONCONTAINER_H
#define OURPAINT_DESKTOP_UI_NOTIFICATIONCONTAINER_H

#include <QWidget>

class QVBoxLayout;
class QScrollArea;

namespace UI {
    class NotificationWidget;


    class NotificationContainer final : public QWidget {
        Q_OBJECT

    public:
        explicit NotificationContainer(QWidget* parent = nullptr);
        void updatePosition();
        void startHideTimer() const;
        void onHideTimeout();
        void addNotification(const QString& text);

    protected:
        void enterEvent(QEnterEvent* event) override;
        void leaveEvent(QEvent* event) override;
        bool eventFilter(QObject* obj, QEvent* event) override;

    public slots:
        void removeNotification(UI::NotificationWidget* widget);

    private:
        QVBoxLayout* containerLayout_{nullptr};
        QVBoxLayout* layout_{nullptr};
        QWidget* containerWidget_{nullptr};
        QScrollArea* scrollArea_{nullptr};
        QList<NotificationWidget*> notifications_;
        QTimer* hideTimer_{nullptr};

        void updateContainerSize();
    };
} // namespace UI
#endif //OURPAINT_DESKTOP_UI_NOTIFICATIONCONTAINER_H
