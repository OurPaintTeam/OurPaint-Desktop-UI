#ifndef OURPAINT_DESKTOP_UI_PAINTERWIDGET_H
#define OURPAINT_DESKTOP_UI_PAINTERWIDGET_H

#include <QWidget>
#include <QStackedWidget>

class QOpenGLWindow;
class QVBoxLayout;


namespace UI {
    class PainterWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit PainterWidget(QWidget* parent = nullptr);
        void setQOpenGL(QOpenGLWindow* renderWindow);
        void setQWindowRender(QWindow* renderWindow);
        void removeQWindowRender(QWindow* renderWindow);

    private:
        QStackedWidget* stackedWidget_;
        std::map<QWindow*, QWidget*> containerMap_;
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_PAINTERWIDGET_H
