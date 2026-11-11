#include "TabBar.h"

#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QSizePolicy>


#include "TabMenu.h"
#include "TabWidget.h"


namespace UI {
    struct FileData;
}


UI::TabBar::TabBar(QWidget *parent)
    : QWidget(parent), tabBar_(new QWidget(this)),
      tabLayout_(new QHBoxLayout(tabBar_)), mainLayout_(new QHBoxLayout(this)),
      scrollArea_(new QScrollArea(this)) {
    setObjectName(QStringLiteral("TabBar"));

    setAcceptDrops(true);
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);

    tabMenu_ = new TabMenu(this);
    connect(tabMenu_, &UI::TabMenu::tabMenuTriggered,
            this, &TabBar::onTabMenuAction);

    tabBar_->setObjectName(QStringLiteral("TabBarWidget"));
    tabBar_->setAttribute(Qt::WA_StyledBackground, true);
    tabBar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabBar_->setMinimumHeight(sizeH);

    mainLayout_->setObjectName(QStringLiteral("TabBarMainLayout"));
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    tabLayout_->setObjectName(QStringLiteral("TabBarLayout"));
    tabLayout_->setContentsMargins(0, 0, 0, 0);
    tabLayout_->setSpacing(3);

    // ScrollArea
    scrollArea_->setObjectName(QStringLiteral("ScrollAreaTabBar"));
    scrollArea_->setWidget(tabBar_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setFrameShape(QFrame::NoFrame);

    // viewport
    scrollArea_->viewport()->setObjectName(QStringLiteral("ViewportScrollAreaTabBar"));
    scrollArea_->viewport()->setAttribute(Qt::WA_StyledBackground, true);

    scrollArea_->horizontalScrollBar()->setObjectName(QStringLiteral("TabBarHScroll"));
    scrollArea_->verticalScrollBar()->setObjectName(QStringLiteral("TabBarVScroll"));
    scrollArea_->horizontalScrollBar()->setAttribute(Qt::WA_StyledBackground, true);
    scrollArea_->verticalScrollBar()->setAttribute(Qt::WA_StyledBackground, true);

    QScroller::grabGesture(scrollArea_->viewport(), QScroller::TouchGesture);

    mainLayout_->addWidget(scrollArea_);

    createPlusButton();
    tabLayout_->addWidget(plusButton_);
    tabLayout_->addStretch();

    translate();
}


void UI::TabBar::onTabMenuAction(UI::tabMenuType type) {
    switch (type) {
        case UI::tabMenuType::deleteTab: {
            if (contextTab_) {
                emit removeTabTriggered(contextTab_->getFileId());
            }
            break;
        }

        case UI::tabMenuType::renameTab: {
            if (contextTab_) {
                contextTab_->startRename();
            }
            break;
        }

        case UI::tabMenuType::closeTab: {
            if (contextTab_) {
                emit closeTabTriggered(contextTab_->getFileId());
            }
            break;
        }

        case UI::tabMenuType::closeAllTabs: {
            QList<qint64> ids;
            for (auto *tab: tabs_) ids.append(tab->getFileId());
            for (qint64 id: ids) emit closeTabTriggered(id);
            break;
        }

        case UI::tabMenuType::closeAllRightTabs: {
            if (!contextTab_) return;
            const int index = tabs_.indexOf(contextTab_);
            if (index == -1) return;

            QList<qint64> ids;
            for (int i = index + 1; i < tabs_.size(); ++i) ids.append(tabs_[i]->getFileId());
            for (qint64 id: ids) emit closeTabTriggered(id);
            break;
        }

        case UI::tabMenuType::closeAllLeftTabs: {
            if (!contextTab_) return;
            const int index = tabs_.indexOf(contextTab_);
            if (index == -1) return;

            QList<qint64> ids;
            for (int i = 0; i < index; ++i) ids.append(tabs_[i]->getFileId());
            for (qint64 id: ids) emit closeTabTriggered(id);
            break;
        }

        case UI::tabMenuType::closeOtherTabs: {
            if (!contextTab_) return;

            QList<qint64> ids;
            for (auto *tab: tabs_) {
                if (tab != contextTab_) ids.append(tab->getFileId());
            }
            for (qint64 id: ids) emit closeTabTriggered(id);
            break;
        }
    }
}


void UI::TabBar::addTabSlot(const FileData& data) {
    auto *tab = new UI::TabWidget(tabBar_, data);
    tabs_.append(tab);

    const auto plusIndex = tabLayout_->indexOf(plusButton_);
    tabLayout_->insertWidget(plusIndex, tab);

    connect(tab, &UI::TabWidget::renameTriggered, this, &UI::TabBar::renameTabTriggered);
    connect(tab, &UI::TabWidget::clickedTriggered, this, &TabBar::setActiveTabSlot);

    connect(tab, &UI::TabWidget::closeRequestedTriggered, this,
            [this](TabWidget *tab) {
                emit closeTabTriggered(tab->getFileId());
            });

    connect(tab, &UI::TabWidget::draggedOutTriggered, this,
            [this](TabWidget *draggedTab) {
                emit openLightEditorWindowTriggered(draggedTab->getFileId());
                emit closeTabTriggered(draggedTab->getFileId());
            });

    connect(tab, &UI::TabWidget::rightClickedTriggered, this, &UI::TabBar::openTabMenuSlot);

    setActiveTabSlot(tab);
}


bool UI::TabBar::isEmptyTab() const {
    return tabs_.isEmpty();
}


void UI::TabBar::renameTab(FileData& data) {
    for (auto *tab: tabs_) {
        if (tab->getFileId() == data.id) {
            tab->setName(data.name);
            return;
        }
    }
}


void UI::TabBar::deleteTabSlot(const qint64 id) {
    for (auto *tab: tabs_) {
        if (tab->getFileId() == id) {
            removeTab(tab);
            return;
        }
    }
}


bool UI::TabBar::removeTab(TabWidget *tab) {
    if (!tab) return false;

    const int index = tabLayout_->indexOf(tab);
    const bool wasActive = (activeTab_ == tab);

    tabLayout_->removeWidget(tab);

    tabs_.removeOne(tab);

    if (wasActive) {
        activeTab_ = nullptr;
    }

    tab->deleteLater();

    if (wasActive) {
        TabWidget *newActive = nullptr;

        if (index >= 0 && index < tabLayout_->count()) {
            newActive = qobject_cast<TabWidget*>(tabLayout_->itemAt(index)->widget());
        }

        if (!newActive && index - 1 >= 0) {
            newActive = qobject_cast<TabWidget*>(tabLayout_->itemAt(index - 1)->widget());
        }

        if (newActive) {
            setActiveTabSlot(newActive);
        } else {
            emit allTabsCloseTriggered();
        }
    }

    return true;
}


UI::TabWidget* UI::TabBar::closeTabSlot(const qint64 id) {
    for (auto *tab: tabs_) {
        if (tab->getFileId() == id) {
            removeTab(tab);
            return nullptr;
        }
    }
    return nullptr;
}


void UI::TabBar::setActiveTabSlot(TabWidget *tab) {
    if (tab == nullptr) {
        return;
    }

    if (activeTab_ != nullptr) {
        activeTab_->setActive(false);
    }

    activeTab_ = tab;
    activeTab_->setActive(true);

    emit setActiveTabTriggered(tab->getFileId());
}


void UI::TabBar::openTabMenuSlot(TabWidget *tab, const QPoint& localPos) {
    if (!tabMenu_) {
        tabMenu_ = new TabMenu(this);
    }

    contextTab_ = tab;
    const QPoint globalPos = tab->mapToGlobal(localPos);
    tabMenu_->move(globalPos);
    tabMenu_->exec();
}


void UI::TabBar::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-tabwidget")) {
        event->acceptProposedAction();
    }
}


void UI::TabBar::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-tabwidget")) {
        event->acceptProposedAction();
    }
}


void UI::TabBar::dropEvent(QDropEvent *event) {
    if (!event->mimeData()->hasFormat("application/x-tabwidget")) {
        return;
    }

    const auto data = event->mimeData()->data("application/x-tabwidget");
    auto *tab = reinterpret_cast<TabWidget*>(data.toULongLong());

    const auto localPos = tabBar_->mapFrom(this, event->position().toPoint());

    quintptr sourceWindowId = 0;
    if (event->mimeData()->hasFormat("application/x-window-id")) {
        sourceWindowId = event->mimeData()->data("application/x-window-id").toULongLong();
    }

    auto targetWindowId = reinterpret_cast<quintptr>(this->window());
    if (sourceWindowId != targetWindowId) {
        return;
    }

    if (const auto plusGeometry = plusButton_->geometry();
        localPos.x() > plusGeometry.center().x()) {
        tabLayout_->removeWidget(tab);
        tabLayout_->insertWidget(tabLayout_->indexOf(plusButton_), tab);
    } else {
        int insertIndex = -1;

        for (int index = 0; index < tabLayout_->count(); ++index) {
            const auto *const widget = tabLayout_->itemAt(index)->widget();
            if (widget == nullptr || widget == plusButton_) {
                continue;
            }

            if (const auto geometry = widget->geometry();
                localPos.x() < geometry.center().x()) {
                insertIndex = index;
                break;
            }
        }

        tabLayout_->removeWidget(tab);

        if (insertIndex != -1) {
            tabLayout_->insertWidget(insertIndex, tab);
        } else {
            tabLayout_->insertWidget(tabLayout_->indexOf(plusButton_), tab);
        }
    }
    setActiveTabSlot(tab);

    event->acceptProposedAction();
}


void UI::TabBar::createPlusButton() {
    plusButton_ = new QPushButton("+", tabBar_);
    constexpr auto sizeW = 32;
    plusButton_->setFixedWidth(sizeW);
    plusButton_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    plusButton_->setObjectName(QStringLiteral("PlusButtonTab"));

    connect(plusButton_, &QPushButton::clicked, this, [this]() {
        createTabSlot();
    });
}


void UI::TabBar::createTabSlot() {
    auto *tab = new TabWidget(this);

    const auto plusIndex = tabLayout_->indexOf(plusButton_);
    tabLayout_->insertWidget(plusIndex, tab);

    tab->startRename();

    connect(tab, &TabWidget::renameTriggered, this,
            [this, tab](FileData& data) {
                emit createTabTriggered(data.name);
                tabLayout_->removeWidget(tab);
                tab->deleteLater();
            });

    connect(tab, &TabWidget::cancelRenameTriggered, this,
            [this, tab]() {
                tabLayout_->removeWidget(tab);
                tab->deleteLater();
            });
}


void UI::TabBar::changeEvent(QEvent *e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}


void UI::TabBar::translate() const {
    if (plusButton_) {
        plusButton_->setToolTip(UI::TabBar::tr("Create new file"));
    }
}
