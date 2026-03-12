#include "ProjectsListWidget.h"

#include <QHBoxLayout>


UI::ProjectsListWidget::ProjectsListWidget(QWidget* parent)
    : QWidget(parent), layout_(new QHBoxLayout(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);

    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
        background: gray;
        border: none;
        border-right: 1px solid #3e3d42;
    )");
}
