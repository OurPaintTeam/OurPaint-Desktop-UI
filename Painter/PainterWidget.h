#ifndef OURPAINT_DESKTOP_UI_PAINTERWIDGET_H
#define OURPAINT_DESKTOP_UI_PAINTERWIDGET_H

#include <QWidget>

class QOpenGLWindow;
class QVBoxLayout;

namespace UI {
    class PainterWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit PainterWidget(QWidget* parent = nullptr);
        void setQOpenGL(QOpenGLWindow* renderWindow);

    private:
        QVBoxLayout* layout_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_PAINTERWIDGET_H
