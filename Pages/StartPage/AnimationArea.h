#ifndef OURPAINT_DESKTOP_UI_ANIMATIONAREA_H
#define OURPAINT_DESKTOP_UI_ANIMATIONAREA_H


#include <QHBoxLayout>
#include <QWidget>

namespace UI {
    class AnimationArea final : public QWidget {
        Q_OBJECT

    public:
        explicit AnimationArea(QWidget* parent = nullptr);

    private:
        QHBoxLayout* layout_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_ANIMATIONAREA_H
