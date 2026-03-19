#ifndef OURPAINT_DESKTOP_UI_CONTAINERWIDGET_H
#define OURPAINT_DESKTOP_UI_CONTAINERWIDGET_H
#include <QWidget>


class QBoxLayout;
class QPushButton;

namespace  UI {
    class SlotWidget;
    class MenuButton;
    enum class ContainerType;


    class ContainerWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit ContainerWidget(ContainerType type,
                                 Qt::Orientation orientation,
                                 QWidget* parent = nullptr);

        [[nodiscard]] ContainerType type() const;
        [[nodiscard]] Qt::Orientation orientation() const;

        void setOrientation(Qt::Orientation o);

        QPushButton* addButton(const QIcon& icon);
        QPushButton* addButton(const QString& text);
        MenuButton* addMenuButton(const QString& text, QMenu* menu);

        void startDrag();

        void setCurrentSlot(SlotWidget* slot);
        [[nodiscard]] SlotWidget* currentSlot() const;

    protected:
        void mousePressEvent(QMouseEvent* e) override;

    private:
        ContainerType type_;
        Qt::Orientation orientation_;

        QBoxLayout* layout_{nullptr};
        SlotWidget* currentSlot_{nullptr};

        void rebuildLayout();
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_CONTAINERWIDGET_H