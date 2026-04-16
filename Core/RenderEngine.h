#ifndef OURPAINT_DESKTOP_UI_RENDERENGINE_H
#define OURPAINT_DESKTOP_UI_RENDERENGINE_H

#include <QWindow>
#include <QBackingStore>

namespace UI {

    class RenderEngine : public QWindow {
        Q_OBJECT

    public:
        explicit RenderEngine(QWindow* parent = nullptr);

        void renderNow();
        void renderLater();

    protected:
        bool event(QEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void exposeEvent(QExposeEvent* event) override;

    private:
        void render();

        QBackingStore backingStore_;
    };

}

#endif