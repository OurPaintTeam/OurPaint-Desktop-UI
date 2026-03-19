#ifndef OURPAINT_DESKTOP_UI_TOOLBAR_H
#define OURPAINT_DESKTOP_UI_TOOLBAR_H

#include <QWidget>

class QHBoxLayout;
class QPushButton;
class QStackedWidget;
class QWheelEvent;


namespace UI {
    class MenuButton;
} // namespace UI

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
        void showAllElements();
        void hideAllElements();

    signals:
        void pointLineDistanceTriggered();
        void pointOnLineTriggered();
        void pointPointDistanceTriggered();
        void coincidentPointsTriggered();
        void lineCircleDistanceTriggered();
        void lineOnCircleTriggered();
        void lineInCircleTriggered();
        void parallelLinesTriggered();
        void perpendicularLinesTriggered();
        void angleBetweenLinesTriggered();

        void pointTriggered();
        void lineTriggered();
        void polylineTriggered();
        void infiniteLineTriggered();

        void circleByDiameterTriggered();
        void circleTwoPointsTriggered();
        void ellipseThreePointsTriggered();

        void arcByRadiusTriggered();
        void arcByDiameterTriggered();
        void arcByThreePointsTriggered();

        void lineSettingsTriggered();
        void circleSettingsTriggered();
        void arcSettingsTriggered();

        void cursorTriggered();
        void sizeTriggered();

    protected:
        void wheelEvent(QWheelEvent* event) override;

    private slots:
        void handlePrimitiveActionSlot(const QAction* action);

    private:
        void initLayout() const;
        void initFiguresLane();
        void initConstraintsLane();
        void initSignals();

        ArcMenu* arcMenu_{nullptr};
        CircleMenu* circleMenu_{nullptr};
        LineMenu* lineMenu_{nullptr};

        QPushButton* cursorTool_{nullptr};
        QPushButton* sizeTool_{nullptr};

        QPushButton* pointPrimitive_{nullptr};
        MenuButton* linePrimitive_{nullptr};
        MenuButton* circlePrimitive_{nullptr};
        MenuButton* arcPrimitive_{nullptr};


        struct ConstraintConfig {
            QString icon;
            QString tooltip;
            std::function<void()> trigger;
        };


        QVector<ConstraintConfig> constraints_;

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
