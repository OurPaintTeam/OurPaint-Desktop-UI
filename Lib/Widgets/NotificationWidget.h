#ifndef OURPAINT_DESKTOP_UI_NOTIFICATIONWIDGET_H
#define OURPAINT_DESKTOP_UI_NOTIFICATIONWIDGET_H

#include <QFrame>

class QPushButton;
class QLabel;
class QHBoxLayout;
class QTimer;

namespace UI {
    class NotificationWidget final : public QFrame  {
        Q_OBJECT

    public:
        explicit NotificationWidget(const QString& text, QWidget *anchorWidget, QWidget *parent = nullptr);

        void showFor(int milliseconds);

    signals:
        void finished();

    protected:
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;

    private slots:
        void onCloseClicked();

    private:
        void updatePosition();

        QHBoxLayout *layout_{nullptr};
        QLabel *label_{nullptr};
        QPushButton *closeButton_{nullptr};
        QTimer *hideTimer_{nullptr};
        QWidget *anchorWidget_{nullptr};
    };
}
#endif
