#include "TabBar.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
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
      scrollArea_(new QScrollArea(this)),
    ownerWindow_(window()){
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
    scrollArea_->viewport()->setAcceptDrops(true);
    scrollArea_->viewport()->installEventFilter(this);
    tabBar_->setAcceptDrops(true);

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
    ownerWindow_ = window();
    auto *tab = new UI::TabWidget(tabBar_, data);
    tab->setOwnerWindow(ownerWindow_);
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

qint64 UI::TabBar::getActiveTabId() const {
    return activeTab_->getFileId();
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
    if (event->mimeData()->hasFormat("application/x-tab-id")) {
        event->acceptProposedAction();
    }
}

void UI::TabBar::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-tab-id")) {
        event->acceptProposedAction();
    }
}

void UI::TabBar::dropEvent(QDropEvent *event) {
    handleDrop(event, tabBar_->mapFrom(this, event->position().toPoint()));
}

bool UI::TabBar::eventFilter(QObject *obj, QEvent *event) {
    if (obj == scrollArea_->viewport()) {
        if (event->type() == QEvent::DragEnter || event->type() == QEvent::DragMove) {
            auto *dragEvent = static_cast<QDragMoveEvent*>(event);
            if (dragEvent->mimeData()->hasFormat("application/x-tab-id")) {
                dragEvent->acceptProposedAction();
                return true;
            }
        }

        if (event->type() == QEvent::Drop) {
            auto *drop = static_cast<QDropEvent*>(event);
            handleDrop(drop, tabBar_->mapFrom(scrollArea_->viewport(), drop->position().toPoint()));
            return drop->isAccepted();
        }
    }

    return QWidget::eventFilter(obj, event);
}

void UI::TabBar::handleDrop(QDropEvent *event, const QPoint& position) {
    if (!event->mimeData()->hasFormat("application/x-tab-id")) return;

    const qint64 fileId = event->mimeData()->data("application/x-tab-id").toLongLong();
    const quintptr sourceWindowPtr = event->mimeData()->data("application/x-window-id").toULongLong();
    const quintptr targetWindowPtr = reinterpret_cast<quintptr>(window());

    if (sourceWindowPtr != targetWindowPtr) {
        emit tabMovedToAnotherWindow(fileId, targetWindowPtr);
        event->acceptProposedAction();
        return;
    }

    TabWidget *tab = nullptr;
    for (auto *candidate : tabs_) {
        if (candidate->getFileId() == fileId) {
            tab = candidate;
            break;
        }
    }

    if (!tab) return;

    int targetIndex = 0;
    for (auto *candidate : tabs_) {
        if (candidate == tab) continue;
        if (position.x() >= candidate->geometry().center().x()) {
            ++targetIndex;
        }
    }

    reorderTab(tab, targetIndex);
    event->acceptProposedAction();
}

void UI::TabBar::reorderTab(TabWidget *tab, int targetIndex) {
    const int sourceIndex = tabs_.indexOf(tab);
    if (sourceIndex < 0) return;

    targetIndex = qBound(0, targetIndex, tabs_.size() - 1);
    if (sourceIndex == targetIndex) return;

    tabs_.move(sourceIndex, targetIndex);
    tabLayout_->removeWidget(tab);
    tabLayout_->insertWidget(targetIndex, tab);

    if (activeTab_) setActiveTabSlot(activeTab_);
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
