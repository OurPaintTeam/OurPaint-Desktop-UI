#include "TabWidget.h"

#include <QApplication>
#include <QDrag>
#include <QHBoxLayout>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>


namespace {
    constexpr int K_TAB_HORIZONTAL_PADDING_PX = 8;
    constexpr int K_TAB_LAYOUT_SPACING_PX = 4;
    constexpr int K_CLOSE_BUTTON_WIDTH_PX = 16;
    constexpr auto K_TAB_MIME_TYPE = "application/x-tabwidget";
} // namespace


UI::TabWidget::TabWidget(const QString& name, QWidget* parent)
    : QWidget(parent)
      , nameButton_(new QPushButton(name, this)), closeButton_(new QToolButton(this)), layout_(new QHBoxLayout(this)),
      name_(name) {
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout_->setContentsMargins(K_TAB_HORIZONTAL_PADDING_PX, 0, K_TAB_HORIZONTAL_PADDING_PX, 0);
    layout_->setSpacing(K_TAB_LAYOUT_SPACING_PX);
    nameButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    closeButton_->setText("✕");
    closeButton_->setFixedWidth(K_CLOSE_BUTTON_WIDTH_PX);
    closeButton_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout_->addWidget(nameButton_);
    layout_->addWidget(closeButton_);

    applyInactiveStyle();

    connect(nameButton_, &QPushButton::clicked, this, [this]() {
        emit clickedTriggered(this);
    });

    connect(closeButton_, &QToolButton::clicked, this, [this]() {
        emit closeRequestedTriggered(this);
    });

    setObjectName(QStringLiteral("TabWidgetActive"));
}


QString UI::TabWidget::getName() const {
    return name_;
}


void UI::TabWidget::setName(const QString& name) {
    name_ = name;
}


void UI::TabWidget::setActive(const bool active) {
    active ? applyActiveStyle() : applyInactiveStyle();
}


void UI::TabWidget::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        dragStartPos_ = e->pos();
    }

    QWidget::mousePressEvent(e);
}


void UI::TabWidget::mouseMoveEvent(QMouseEvent* e) {
    if (!(e->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((e->pos() - dragStartPos_).manhattanLength()
        < QApplication::startDragDistance()) {
        return;
    }

    auto drag = std::make_unique<QDrag>(this);
    auto mime = std::make_unique<QMimeData>();

    mime->setData(K_TAB_MIME_TYPE, QByteArray::number(reinterpret_cast<quintptr>(this)));
    drag->setMimeData(mime.release());

    if (const auto action = drag->exec(Qt::MoveAction);
        action == Qt::IgnoreAction) {
        emit draggedOutTriggered(this);
    }
}


void UI::TabWidget::applyInactiveStyle() {
    nameButton_->setObjectName("NameButtonTabInActive");
    closeButton_->setObjectName("CloseButtonTabInActive");
    setObjectName(QStringLiteral("TabWidgetInActive"));

    style()->unpolish(this);
    style()->polish(this);
    update();
}


void UI::TabWidget::applyActiveStyle() {
    nameButton_->setObjectName("NameButtonTabActive");
    closeButton_->setObjectName("CloseButtonTabActive");
    setObjectName(QStringLiteral("TabWidgetActive"));

    style()->unpolish(this);
    style()->polish(this);
    update();
}
