#ifndef OURPAINT_DESKTOP_UI_SLOTWIDGET_H
#define OURPAINT_DESKTOP_UI_SLOTWIDGET_H

#include <QWidget>
class QBoxLayout;

namespace UI {
    class ContainerWidget;
    class RotatedLabel;
    enum class ContainerType;


    class SlotWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit SlotWidget(const QSet<ContainerType>& acceptedTypes,
                            Qt::Orientation orientation,
                            QWidget* parent = nullptr);

        void insert(ContainerWidget* c);

        signals:
            void migrateContainer(UI::ContainerWidget* container);

    protected:
        void dragEnterEvent(QDragEnterEvent* e) override;
        void dropEvent(QDropEvent* e) override;

    private:
        QSet<ContainerType> acceptedTypes_;
        Qt::Orientation orientation_;

        QBoxLayout* layout_{nullptr};

        QList<ContainerWidget*> containers_;
        RotatedLabel* placeholder_{nullptr};

        bool canAccept(const ContainerWidget* c) const;
        static ContainerWidget* containerFromEvent(const QDropEvent* e);

        void removeContainer(ContainerWidget* c);
        void createPlaceholder();
        void removePlaceholder();
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_SLOTWIDGET_H