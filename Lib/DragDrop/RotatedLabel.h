#ifndef OURPAINT_DESKTOP_UI_ROTATEDLABEL_H
#define OURPAINT_DESKTOP_UI_ROTATEDLABEL_H
#include <QLabel>


namespace UI {
    class RotatedLabel final : public QLabel {
    public:
        explicit RotatedLabel(const QString& text, QWidget* parent = nullptr);

        void setAngle(int angle);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        int angle_{0};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_ROTATEDLABEL_H
