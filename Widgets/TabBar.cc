#include "TabBar.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QScrollArea>
#include <QScroller>
#include <QSizePolicy>

#include "TabWidget.h"


UI::TabBar::TabBar(QWidget* parent)
    : QWidget(parent), tabBar_(new QWidget(this)), tabLayout_(new QHBoxLayout(tabBar_)),
      mainLayout_(new QHBoxLayout(this)), scrollArea_(new QScrollArea(this)) {
    setAcceptDrops(true);
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "background: #494850;"
        "border-bottom: 1px solid #3e3d42;"
        "border-left: 1px solid #3e3d42;"
        "border-right: 1px solid #3e3d42;"
    );

    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    // Container for tab widgets.
    tabLayout_->setContentsMargins(0, 0, 0, 0);
    constexpr auto spacing = 3;
    tabLayout_->setSpacing(spacing);

    // Scroll area for overflowing tab bar.
    scrollArea_->setWidget(tabBar_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setStyleSheet("QScrollArea { background: transparent; }");

    QScroller::grabGesture(scrollArea_->viewport(), QScroller::LeftMouseButtonGesture);

    mainLayout_->addWidget(scrollArea_);

    createPlusButton();
    tabLayout_->addWidget(plusButton_);
    tabLayout_->addStretch();
    setObjectName(QStringLiteral("tabbar"));
}


void UI::TabBar::addTab(const QString& name) {
    auto* tab = new UI::TabWidget(name, tabBar_);
    tabs_.append(tab);

    const auto plusIndex = tabLayout_->indexOf(plusButton_);
    tabLayout_->insertWidget(plusIndex, tab);

    connect(tab, &UI::TabWidget::clicked, this, &TabBar::setActiveTab);
    connect(tab, &UI::TabWidget::closeRequested, this, &TabBar::removeTab);
    connect(tab, &UI::TabWidget::draggedOut, this, [this](TabWidget* draggedTab) {
        emit openTabWindow(draggedTab->name());
        removeTab(draggedTab);
    });

    setActiveTab(tab);
}


void UI::TabBar::setActiveTab(TabWidget* tab) {
    if (tab == nullptr) {
        return;
    }

    if (activeTab_ != nullptr) {
        activeTab_->setActive(false);
    }

    activeTab_ = tab;
    activeTab_->setActive(true);
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

    qint32 insertIndex = -1;

    for (qint32 index = 0; index < tabLayout_->count(); ++index) {
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

    // If mouse is to the right of all tabs, insert before "+".
    if (insertIndex == -1) {
        insertIndex = tabLayout_->indexOf(plusButton_);
    }

    tabLayout_->removeWidget(tab);
    tabLayout_->insertWidget(insertIndex, tab);

    setActiveTab(tab);

    event->acceptProposedAction();
}


void UI::TabBar::createPlusButton() {
    plusButton_ = new QPushButton("+", tabBar_);
    constexpr auto sizeW = 32;
    plusButton_->setFixedWidth(sizeW);
    plusButton_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    plusButton_->setStyleSheet(
        "QPushButton {"
        " background-color: #615760;"
        " color: #D8D8F6;"
        "border: none;"
        " border-top-left-radius: 5px;"
        " border-top-right-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: #7a6f79; }"
    );

    connect(plusButton_, &QPushButton::clicked, this, [this]() {
        addTab("New Tab");
    });
}


void UI::TabBar::removeTab(TabWidget* tab) {
    if (tab == nullptr) {
        return;
    }

    tabLayout_->removeWidget(tab);
    tabs_.removeOne(tab);

    if (activeTab_ == tab) {
        activeTab_ = nullptr;
        if (!tabs_.isEmpty()) {
            setActiveTab(tabs_.last());
        }
    }

    tab->deleteLater();
}
