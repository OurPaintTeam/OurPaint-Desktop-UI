#ifndef OURPAINT_DESKTOP_UI_TABBAR_H
#define OURPAINT_DESKTOP_UI_TABBAR_H


#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>


namespace UI {
    class TabWidget;


    // Scrollable bar with draggable document tabs.
    class TabBar final : public QWidget {
        Q_OBJECT

    public:
        explicit TabBar(QWidget* parent = nullptr);

        void addTab(const QString& name);

    public slots:
        void setActiveTab(UI::TabWidget* tab);

    signals:
        void openTabWindow(const QString& tab);

    protected:
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;

    private:
        QWidget* tabBar_{nullptr};
        QHBoxLayout* tabLayout_{nullptr};
        QPushButton* plusButton_{nullptr};
        QHBoxLayout* mainLayout_{nullptr};
        QScrollArea* scrollArea_{nullptr};

        QVector<TabWidget*> tabs_{};
        TabWidget* activeTab_{nullptr};

        void createPlusButton();
        void removeTab(TabWidget* tab);
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABBAR_H
