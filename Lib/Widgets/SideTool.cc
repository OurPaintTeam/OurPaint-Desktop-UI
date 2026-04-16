#include "SideTool.h"

#include <QVBoxLayout>

#include "SlotWidget.h"
#include "ToolsType.h"


UI::SideTool::SideTool(QWidget* parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this)) {
    constexpr auto sizeW = 32;
    setFixedWidth(sizeW);

    setAttribute(Qt::WA_StyledBackground, true);

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    slot_ = new UI::SlotWidget(
        {ContainerType::Tools},
        Qt::Vertical,
        this
    );
    setObjectName("SideTool");

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
