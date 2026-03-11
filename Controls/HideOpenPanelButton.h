#ifndef OURPAINT_DESKTOP_UI_HIDEOPENPANELBUTTON_H
#define OURPAINT_DESKTOP_UI_HIDEOPENPANELBUTTON_H
#include <QPushButton>

namespace UI {
    class HideOpenPanelButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit HideOpenPanelButton(QWidget* parent = nullptr);

        void setVertical(bool v);
        [[nodiscard]] bool isVertical() const;

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        bool vertical_{false};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_HIDEOPENPANELBUTTON_H
