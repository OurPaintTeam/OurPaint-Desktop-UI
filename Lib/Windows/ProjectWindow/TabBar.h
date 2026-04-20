#ifndef OURPAINT_DESKTOP_UI_TABBAR_H
#define OURPAINT_DESKTOP_UI_TABBAR_H


#include <QWidget>
#include <QPointer>

class QHBoxLayout;
class QPushButton;
class QScrollArea;

namespace UI {
class TabMenu;
class TabWidget;


    // Scrollable bar with draggable document tabs.
    class TabBar final : public QWidget {
        Q_OBJECT

    public:
        explicit TabBar(QWidget* parent = nullptr);
        [[nodiscard]] bool isEmptyTab() const;
    signals:
        void renameTabTriggered(const QString& oldName, const QString& newName);
        void removeTabTriggered(const QString& name);
        void createTabTriggered(const QString& name);
        void setActiveTabTriggered(const QString& name);
        void openTabWindowTriggered(const QString& tab);
        void allTabsCloseTriggered();

    public slots:
        void renameTab(const QString& oldName, const QString& newName);
        void deleteTabSlot(const QString& name);
        UI::TabWidget* closeTabOnNameSlot(const QString& name);
        void addTabSlot(const QString& name);
        void createTabSlot();

    private slots:
        void setActiveTabSlot(UI::TabWidget* tab);
        void openTabMenuSlot(TabWidget* tab, const QPoint& localPos);

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
        TabMenu* tabMenu_{nullptr};
        QPointer<TabWidget> contextTab_;

        void createPlusButton();
      bool removeTab(TabWidget* tab);
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABBAR_H
