#ifndef OURPAINT_DESKTOP_UI_NOTIFICATIONWIDGET_H
#define OURPAINT_DESKTOP_UI_NOTIFICATIONWIDGET_H

#include <QWidget>

class QPushButton;
class QLabel;
class QHBoxLayout;

namespace UI {

    class NotificationWidget final : public QWidget
    {
        Q_OBJECT
    public:
        explicit NotificationWidget(const QString &text, QWidget *parent = nullptr);
        signals:
            void deleted(UI::NotificationWidget *widget);

    private slots:
        void onDeleteClicked();
    private:
        QHBoxLayout *layout_{nullptr};
        QLabel *label_{nullptr};
        QPushButton *closeButton_{nullptr};
    };

} // namespace UI

#endif //OURPAINT_DESKTOP_UI_NOTIFICATIONWIDGET_H