#ifndef OURPAINT_DESKTOP_UI_DRAGGLECONTAINER_H
#define OURPAINT_DESKTOP_UI_DRAGGLECONTAINER_H

#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QWidget>

namespace UI {
    class MenuButton;
    class SlotWidget;

    /* container types */

    enum class ContainerType : qint8 {
        Tools,
        Figures,
        Requirements,
        Tabs,
        Side
    };


    /* button with right-click signal */

    class DoubleClickButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit DoubleClickButton(const QString& text,
                                   QWidget* parent = nullptr);

    signals:
        void rightClicked();

    protected:
        void mousePressEvent(QMouseEvent* e) override;
    };


    /* draggable container */

    class ContainerWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit ContainerWidget(ContainerType type,
                                 Qt::Orientation orientation,
                                 QWidget* parent = nullptr);

        [[nodiscard]] ContainerType type() const;
        [[nodiscard]] Qt::Orientation orientation() const;

        void setOrientation(Qt::Orientation o);

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


    /* drop slot */

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
        QLabel* placeholder_{nullptr};

        bool canAccept(const ContainerWidget* c) const;
        static ContainerWidget* containerFromEvent(const QDropEvent* e);

        void removeContainer(ContainerWidget* c);
        void createPlaceholder();
        void removePlaceholder();
    };
} // namespace UI
#endif
