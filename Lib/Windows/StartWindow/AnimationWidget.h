#ifndef OURPAINT_DESKTOP_UI_ANIMATIONWIDGET_H
#define OURPAINT_DESKTOP_UI_ANIMATIONWIDGET_H


#include <QWidget>

class QLabel;

namespace UI {
    class AnimationWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit AnimationWidget(QWidget* parent = nullptr);

    protected:
        void resizeEvent(QResizeEvent* event) override;
        void paintEvent(QPaintEvent* event) override;
        void changeEvent(QEvent *e) override;

    private:
        void translate() const;
        QLabel* label_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_ANIMATIONWIDGET_H
