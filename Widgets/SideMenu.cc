#include "SideMenu.h"

#include "DraggleContainer.h"
#include "InformationPanel.h"
#include "MessengerPanel.h"


UI::SideMenu::SideMenu(QWidget* parent)
    : QWidget(parent), stack_(new QStackedLayout(this)), infoPanel_(new UI::InformationPanel(this)),
      msgPanel_(new UI::MessengerPanel(this)) {
    constexpr auto sizeH = 32;
    setFixedWidth(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(R"(
            QWidget {
                background: #494850;
            }

            QWidget#sideSlot {
                border: 1px dashed #666;
            }

            QPushButton {
                background: transparent;
                border: none;
                color: #D8D8F6;
                border-radius: 6px;
                padding: 6px;
            }

            QPushButton:hover {
                background: rgba(255,255,255,0.08);
            }

            QPushButton:pressed {
                background: rgba(255,255,255,0.16);
            }
        )");


    stack_->setContentsMargins(0, 0, 0, 0);
    stack_->setSpacing(0);

    slot_ = new SlotWidget(
        {
            UI::ContainerType::Side, UI::ContainerType::Tools, UI::ContainerType::Requirements,
            UI::ContainerType::Figures
        },
        Qt::Vertical,
        this
    );
    slot_->setObjectName("sideSlot");

    stack_->addWidget(slot_); // Icons
    stack_->addWidget(infoPanel_);
    stack_->addWidget(msgPanel_);

    stack_->setCurrentWidget(slot_);
    connect(infoPanel_->getButtonBack(), &QPushButton::clicked, this, [this]() {
        showMenu();
    });
    connect(msgPanel_->getButtonBack(), &QPushButton::clicked, this, [this]() {
        showMenu();
    });
}


void UI::SideMenu::setExpanded(const bool expanded) {
    constexpr auto sizeH1 = 32;
    constexpr auto sizeH2 = 260;
    setFixedWidth(expanded ? sizeH2 : sizeH1);
}


void UI::SideMenu::addContainer(ContainerWidget* c) const {
    slot_->insert(c);
}


void UI::SideMenu::openInfPanel() {
    setExpanded(true);
    stack_->setCurrentWidget(infoPanel_);
    infoPanel_->show();
}


void UI::SideMenu::openMesPanel() {
    setExpanded(true);
    stack_->setCurrentWidget(msgPanel_);
    msgPanel_->show();
}


UI::SlotWidget* UI::SideMenu::slotWidget() const {
    return slot_;
}


void UI::SideMenu::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
}


void UI::SideMenu::showMenu() {
    setExpanded(false);
    stack_->setCurrentWidget(slot_);
}
