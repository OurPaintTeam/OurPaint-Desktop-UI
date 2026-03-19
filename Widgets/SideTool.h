#ifndef OURPAINT_DESKTOP_UI_SIDETOOL_H
#define OURPAINT_DESKTOP_UI_SIDETOOL_H

#include <QWidget>

class QVBoxLayout;

namespace UI {
    class SlotWidget;
    class ContainerWidget;


    class SideTool final : public QWidget {
        Q_OBJECT

    public:
        explicit SideTool(QWidget* parent = nullptr);
        void addContainer(ContainerWidget* c) const;
        [[nodiscard]] UI::SlotWidget* slotWidget() const;

    private:
        SlotWidget* slot_{nullptr};
        QVBoxLayout* layout_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_SIDETOOL_H
