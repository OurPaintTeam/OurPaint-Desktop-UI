#ifndef OURPAINT_DESKTOP_UI_BASEPAGE_H
#define OURPAINT_DESKTOP_UI_BASEPAGE_H

#include <QWidget>

class QVBoxLayout;

namespace UI {
    class BasePage : public QWidget {
    public:
        explicit BasePage(QWidget* parent = nullptr);
        ~BasePage() override = default;

    protected:
        void initBasePage();

        QVBoxLayout* mainLayout_{nullptr};
        QWidget* mainArea_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEPAGE_H
