#ifndef OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H
#define OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QPoint>


class QMouseEvent;

namespace UI {
    // Base window without system frame
    class FramelessWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit FramelessWindow(QWidget* parent = nullptr);
        ~FramelessWindow() override = default;

        virtual void initUI() = 0;

    protected:
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

#ifdef Q_OS_WIN
        bool nativeEvent(const QByteArray& eventType,
                         void* message,
                         qintptr* result) override;
#endif

    private:
        QPoint dragOffset_{};
        bool isDragging_ = false;
    };
}

#endif // OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H
