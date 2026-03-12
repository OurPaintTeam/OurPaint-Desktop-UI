#ifndef OURPAINT_DESKTOP_UI_TOOLBAR_H
#define OURPAINT_DESKTOP_UI_TOOLBAR_H

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QWheelEvent>
#include <QWidget>

namespace UI {
    class SlotWidget;
    class ContainerWidget;
    class LineMenu;
    class CircleMenu;
    class ArcMenu;


    // Horizontal toolbar with tool lanes.
    class ToolBar final : public QWidget {
        Q_OBJECT

    public:
        explicit ToolBar(QWidget* parent = nullptr);

    protected:
        void wheelEvent(QWheelEvent* event) override;

    private:
        ArcMenu* arcMenu_{nullptr};
        CircleMenu* circleMenu_{nullptr};
        LineMenu* lineMenu_{nullptr};

        QHBoxLayout* layout_{nullptr};
        QStackedWidget* stack_{nullptr};
        QHBoxLayout* mainLayout_{nullptr};
        QWidget* firstLaneWidget_{nullptr};
        QHBoxLayout* firstLayout_{nullptr};
        QWidget* secondLaneWidget_{nullptr};
        QHBoxLayout* secondLayout_{nullptr};
        ContainerWidget* toolsContainer_{nullptr};
        SlotWidget* figuresSlot_{nullptr};
        ContainerWidget* figuresContainer_{nullptr};
        SlotWidget* reqsSlot_{nullptr};
        ContainerWidget* reqsContainer_{nullptr};

        bool isFirstLaneVisible_{false};
    };
} // namespace UI


#endif // OURPAINT_DESKTOP_UI_TOOLBAR_H
