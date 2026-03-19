#ifndef OURPAINT_DESKTOP_UI_MESSENGERPANEL_H
#define OURPAINT_DESKTOP_UI_MESSENGERPANEL_H

#include <QWidget>

class QHBoxLayout;
class QPushButton;

namespace UI {
    class MessengerPanel final : public QWidget {
        Q_OBJECT

    public:
        explicit MessengerPanel(QWidget* parent = nullptr);
        void createButtonBack();
        QPushButton* getButtonBack() const;

    private:
        QHBoxLayout* layout_{nullptr};
        QPushButton* btnBack_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_MESSENGERPANEL_H
