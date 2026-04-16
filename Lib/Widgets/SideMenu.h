#ifndef OURPAINT_DESKTOP_UI_SIDEMENU_H
#define OURPAINT_DESKTOP_UI_SIDEMENU_H

#include <QWidget>

class QStackedLayout;

namespace UI {
    class MessengerPanel;
    class InformationPanel;
    class ContainerWidget;
    class SlotWidget;


    class SideMenu final : public QWidget {
        Q_OBJECT

    public:
        explicit SideMenu(QWidget* parent = nullptr);
        void setExpanded(bool expanded);
        void addContainer(ContainerWidget* c) const;
        void openInfPanel();
        void openMesPanel();

        [[nodiscard]] SlotWidget* slotWidget() const;

    protected:
        void resizeEvent(QResizeEvent* e) override;
        void showMenu();

    private:
        QStackedLayout* stack_{nullptr};
        SlotWidget* slot_{nullptr};

        InformationPanel* infoPanel_{nullptr};
        MessengerPanel* msgPanel_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_SIDEMENU_H
