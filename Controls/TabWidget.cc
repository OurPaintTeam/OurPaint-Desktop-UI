#include "TabWidget.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>


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

    nameButton_->setStyleSheet("background: transparent; border: none;");
    nameButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    closeButton_->setText("✕");
    closeButton_->setFixedWidth(K_CLOSE_BUTTON_WIDTH_PX);
    closeButton_->setStyleSheet("background: transparent; border: none;");
    closeButton_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout_->addWidget(nameButton_);
    layout_->addWidget(closeButton_);

    applyInactiveStyle();

    connect(nameButton_, &QPushButton::clicked, this, [this]() {
        emit clicked(this);
    });

    connect(closeButton_, &QToolButton::clicked, this, [this]() {
        emit closeRequested(this);
    });

    setObjectName(QStringLiteral("TabWidget"));
}


QString UI::TabWidget::getName() const {
    return name_;
}


QString UI::TabWidget::name() const {
    return name_;
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

    auto* drag = new QDrag(this);
    auto* mime = new QMimeData;

    mime->setData(K_TAB_MIME_TYPE, QByteArray::number(reinterpret_cast<quintptr>(this)));
    drag->setMimeData(mime);

    if (const auto action = drag->exec(Qt::MoveAction);
        action == Qt::IgnoreAction) {
        emit draggedOut(this);
    }
}


void UI::TabWidget::applyInactiveStyle() {
    // Container background.
    setStyleSheet(
        "background-color: #615760;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        "border: none;"
    );

    // Title button.
    nameButton_->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            color: #D8D8F6;
            font-size: 9pt;
        }
        QPushButton:hover {
            color: #FFFFFF;
        }
    )");

    // Close button.
    closeButton_->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            color: #D8D8F6;
        }
        QToolButton:hover {
            color: #FFFFFF;
        }
    )");
}


void UI::TabWidget::applyActiveStyle() {
    setStyleSheet(
        "background-color: #978897;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        "border-bottom: 2px solid #978897;"
        "border: none;"
    );

    nameButton_->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            color: #D8D8F6;
            font-size: 9pt;
            font-weight: bold;
        }
        QPushButton:hover {
            color: #FFFFFF;
        }
    )");

    closeButton_->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            color: #D8D8F6;
        }
        QToolButton:hover {
            color: #FFFFFF;
        }
    )");
}