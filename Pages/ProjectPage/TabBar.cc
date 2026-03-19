#include "TabBar.h"

#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QSizePolicy>

#include "InputWidget.h"
#include "TabWidget.h"


UI::TabBar::TabBar(QWidget* parent)
    : QWidget(parent),
      tabBar_(new QWidget(this)),
      tabLayout_(new QHBoxLayout(tabBar_)),
      mainLayout_(new QHBoxLayout(this)),
      scrollArea_(new QScrollArea(this)) {
    setObjectName(QStringLiteral("TabBar"));

    setAcceptDrops(true);
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);

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

    QScroller::grabGesture(scrollArea_->viewport(), QScroller::LeftMouseButtonGesture);

    mainLayout_->addWidget(scrollArea_);

    createPlusButton();
    tabLayout_->addWidget(plusButton_);
    tabLayout_->addStretch();
}


void UI::TabBar::addTab(const QString& name) {
    if (name.isEmpty()) {
        return;
    }
    for (const auto& ch: name) {
        if (ch.isSpace()) {
            return;
        }
    }
    for (const auto& t: tabs_) {
        if (t->getName() == name) {
            return;
        }
    }

    auto* tab = new UI::TabWidget(name, tabBar_);
    tabs_.append(tab);

    const auto plusIndex = tabLayout_->indexOf(plusButton_);
    tabLayout_->insertWidget(plusIndex, tab);

    connect(tab, &UI::TabWidget::clickedTriggered, this, &TabBar::setActiveTabSlot);
    connect(tab, &UI::TabWidget::closeRequestedTriggered, this, &TabBar::closeTabOnPtrSlot);
    connect(tab, &UI::TabWidget::draggedOutTriggered, this, [this](TabWidget* draggedTab) {
        emit openTabWindowTriggered(draggedTab->getName());
        closeTabOnPtrSlot(draggedTab);
    });

    setActiveTabSlot(tab);
}


bool UI::TabBar::isEmptyTab() const {
    return tabLayout_->count() == 2; // (spacer + plusButton)
}


void UI::TabBar::renameTab(const QString& oldName, const QString& newName) {
    for (auto* tab: tabs_) {
        if (tab->getName() == oldName) {
            tab->setName(newName);
            return;
        }
    }
}


void UI::TabBar::deleteTabSlot(const QString& name) {
    for (auto* tab: tabs_) {
        if (tab->getName() == name) {
            tabLayout_->removeWidget(tab);
            tabs_.removeOne(tab);

            if (activeTab_ == tab) {
                activeTab_ = nullptr;
            }

            tab->deleteLater();
            return;
        }
    }
}


void UI::TabBar::openTabSlot(const QString& name) {
    for (auto* tab: tabs_) {
        if (tab->getName() == name) {
            if (!tab->isVisible()) {
                const auto plusIndex = tabLayout_->indexOf(plusButton_);

                tab->show();
                tabLayout_->insertWidget(plusIndex, tab);
            }

            setActiveTabSlot(tab);
            return;
        }
    }
}


void UI::TabBar::createTabSlot(const QString& name) {
    if (name.isEmpty()) {
        return;
    }

    for (const auto& ch: name) {
        if (ch.isSpace()) {
            return;
        }
    }

    for (const auto& t: tabs_) {
        if (t->getName() == name) {
            return;
        }
    }

    auto* tab = new UI::TabWidget(name, tabBar_);
    tabs_.append(tab);

    const auto plusIndex = tabLayout_->indexOf(plusButton_);
    tabLayout_->insertWidget(plusIndex, tab);

    connect(tab, &UI::TabWidget::clickedTriggered,
            this, &TabBar::setActiveTabSlot);

    connect(tab, &UI::TabWidget::closeRequestedTriggered,
            this, &TabBar::closeTabOnPtrSlot);

    connect(tab, &UI::TabWidget::draggedOutTriggered,
            this,
            [this](TabWidget* draggedTab) {
                emit openTabWindowTriggered(draggedTab->getName());
                closeTabOnPtrSlot(draggedTab);
            });

    setActiveTabSlot(tab);
}


UI::TabWidget* UI::TabBar::closeTabOnNameSlot(const QString& name) {
    for (auto* tab: tabs_) {
        if (tab->getName() == name) {
            closeTabOnPtrSlot(tab);
            return tab;
        }
    }

    return nullptr;
}


void UI::TabBar::setActiveTabSlot(TabWidget* tab) {
    if (tab == nullptr) {
        return;
    }

    if (activeTab_ != nullptr) {
        activeTab_->setActive(false);
    }

    activeTab_ = tab;
    activeTab_->setActive(true);

    emit setActiveTabTriggered(tab->getName());
}


void UI::TabBar::closeTabOnPtrSlot(TabWidget* tab) {
    if (!tab) {
        return;
    }

    const auto index = tabLayout_->indexOf(tab);

    tabLayout_->removeWidget(tab);
    tab->hide();

    if (activeTab_ == tab) {
        activeTab_ = nullptr;

        if (isEmptyTab()) {
            emit allTabsCloseTriggered();
            return;
        }

        QWidget* newTab = nullptr;

        if (index < tabLayout_->count()) {
            newTab = tabLayout_->itemAt(index)->widget();
        } else if (index - 1 >= 0) {
            newTab = tabLayout_->itemAt(index - 1)->widget();
        }

        if (newTab) {
            setActiveTabSlot(dynamic_cast<TabWidget*>(newTab));
        }
    }
}


void UI::TabBar::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-tabwidget")) {
        event->acceptProposedAction();
    }
}


void UI::TabBar::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasFormat("application/x-tabwidget")) {
        event->acceptProposedAction();
    }
}


void UI::TabBar::dropEvent(QDropEvent* event) {
    if (!event->mimeData()->hasFormat("application/x-tabwidget")) {
        return;
    }

    const auto data = event->mimeData()->data("application/x-tabwidget");
    auto* tab = reinterpret_cast<TabWidget*>(data.toULongLong());

    const auto localPos = tabBar_->mapFrom(this, event->position().toPoint());

    if (const auto plusGeometry = plusButton_->geometry(); localPos.x() > plusGeometry.center().x()) {
        tabLayout_->removeWidget(tab);
        tabLayout_->insertWidget(tabLayout_->indexOf(plusButton_), tab);
    } else {
        int insertIndex = -1;

        for (int index = 0; index < tabLayout_->count(); ++index) {
            const auto* const widget = tabLayout_->itemAt(index)->widget();
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
        auto* prompt = new InputWidget("Name:", this);
        const auto globalPos = plusButton_->mapToGlobal(QPoint(0, plusButton_->height()));

        prompt->move(globalPos);
        prompt->show();

        connect(prompt, &InputWidget::inputEnteredTriggered, this, [this](const QString& text) {
            emit createTabTriggered(text);
        });
    });
}
