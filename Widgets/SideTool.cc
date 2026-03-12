#include "SideTool.h"

#include "DraggleContainer.h"


UI::SideTool::SideTool(QWidget* parent)
    : QWidget(parent), layout_(new QVBoxLayout(this)) {
    constexpr auto sizeW = 32;
    setFixedWidth(sizeW);

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(R"(
            QWidget {
                background: transparent;
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

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    slot_ = new UI::SlotWidget(
        {ContainerType::Tools, ContainerType::Requirements, ContainerType::Figures},
        Qt::Vertical,
        this
    );
    slot_->setObjectName("sideSlot");

    layout_->addWidget(slot_);
}


void UI::SideTool::addContainer(ContainerWidget* c) const {
    if (c != nullptr && slot_ != nullptr) {
        slot_->insert(c);
    }
}


UI::SlotWidget* UI::SideTool::slotWidget() const {
    return slot_;
}
