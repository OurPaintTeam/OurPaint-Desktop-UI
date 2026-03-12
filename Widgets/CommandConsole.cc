#include "CommandConsole.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>


UI::CommandConsole::CommandConsole(QWidget* parent)
    : QWidget(parent), layout_(new QHBoxLayout(this)), lineEdit_(new QLineEdit(this)) {
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
        background-color: #3e3d3d;
        border: 1px solid black;
        border-top-left-radius: 5px;
        border-top-right-radius: 5px;
    )");
    constexpr auto margins = 10;
    layout_->setContentsMargins(margins, 0, margins, 0);
    layout_->setSpacing(0);
    button_ = new QPushButton("▶", this);
    lineEdit_->setStyleSheet(R"(
    QLineEdit {
        background-color: transparent;
        color: #D8D8F6;
        border: none;

        padding-left: 6px;
        padding-right: 6px;

        font-size: 14px;

        padding-top: 1px;     /* уменьшай если нужно выше */
        padding-bottom: 1px;  /* минимальный отступ снизу */
    }
)");

    button_->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #D8D8F6;
            border: none;
            padding-left: 8px;
            padding-right: 8px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 20);
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 40);
        }
    )");

    lineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout_->addWidget(lineEdit_);
    layout_->addWidget(button_);
    setObjectName(QStringLiteral("commandconsole"));
}
