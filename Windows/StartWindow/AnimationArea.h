#ifndef OURPAINT_DESKTOP_UI_ANIMATIONAREA_H
#define OURPAINT_DESKTOP_UI_ANIMATIONAREA_H

#include <QWidget>

class QHBoxLayout;

namespace UI {
    class AnimationWidget;


    class AnimationArea final : public QWidget {
        Q_OBJECT

    public:
        explicit AnimationArea(QWidget* parent = nullptr);

    private:
        QHBoxLayout* layout_{nullptr};
        AnimationWidget* animationWidget_{nullptr};
    };
} // namespace UI

#endif
