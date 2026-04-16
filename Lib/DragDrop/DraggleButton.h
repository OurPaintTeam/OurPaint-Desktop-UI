#ifndef OURPAINT_DESKTOP_UI_DRAGGLEBUTTON_H
#define OURPAINT_DESKTOP_UI_DRAGGLEBUTTON_H
#include <QPushButton>

namespace UI {
    class DraggleButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit DraggleButton(const QString& text,
                               QWidget* parent = nullptr);
        DraggleButton(const QIcon& icon, QWidget* parent);

    signals:
        void rightClicked();

    protected:
        void mousePressEvent(QMouseEvent* e) override;
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_DRAGGLEBUTTON_H
