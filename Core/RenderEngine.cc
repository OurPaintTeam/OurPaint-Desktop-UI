#include "RenderEngine.h"
#include <QPainter>

namespace UI {

    RenderEngine::RenderEngine(QWindow* parent)
        : QWindow(parent), backingStore_(this) {
        setSurfaceType(QSurface::RasterSurface);
    }


    void RenderEngine::renderNow() {
        if (!isExposed())
            return;
        render();
    }

    void RenderEngine::renderLater() {
        requestUpdate();
    }

    bool RenderEngine::event(QEvent* event) {
        if (event->type() == QEvent::UpdateRequest) {
            renderNow();
            return true;
        }
        return QWindow::event(event);
    }

    void RenderEngine::exposeEvent(QExposeEvent*) {
        renderNow();
    }

    void RenderEngine::resizeEvent(QResizeEvent*) {
        backingStore_.resize(size());
        if (isExposed())
            renderNow();
    }

    void RenderEngine::render() {
        if (size().isEmpty())
            return;

        backingStore_.beginPaint(QRect(QPoint(0, 0), size()));

        QPaintDevice* device = backingStore_.paintDevice();
        QPainter painter(device);

        painter.fillRect(QRect(QPoint(0, 0), size()), Qt::white);

        painter.end();
        backingStore_.endPaint();

        backingStore_.flush(QRect(QPoint(0, 0), size()));
    }

}