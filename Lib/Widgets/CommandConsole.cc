#include "CommandConsole.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>


UI::CommandConsole::CommandConsole(QWidget* parent)
    : QWidget(parent), layout_(new QHBoxLayout(this)), lineEdit_(new QLineEdit(this)), button_(new QPushButton(this)) {
    constexpr auto sizeH = 32;
    setFixedHeight(sizeH);
    setAttribute(Qt::WA_StyledBackground, true);

    constexpr auto margins = 10;
    layout_->setContentsMargins(margins, 0, 0, 0);
    layout_->setSpacing(0);
    constexpr auto bSize = QSize(25, 25);
    button_->setIconSize(QSize(bSize));
    button_->setIcon(QIcon(":/Assets/icons/console/enter.ico"));
    button_->setToolTip("Enter command");
    lineEdit_->setObjectName(QStringLiteral("CommandLineEdit"));
    button_->setObjectName(QStringLiteral("CommandEnter"));

    lineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    lineEdit_->installEventFilter(this);

    connect(button_, &QPushButton::clicked, this, [this]() {
        emit sentCommandTriggered(lineEdit_->text());
        lineEdit_->clear();
    });
    connect(lineEdit_, &QLineEdit::returnPressed, this, [this]() {
        emit sentCommandTriggered(lineEdit_->text());
        lineEdit_->clear();
    });

    layout_->addWidget(lineEdit_);
    layout_->addWidget(button_);
    setObjectName(QStringLiteral("CommandConsole"));
}


void UI::CommandConsole::setLineEditEngine(QLineEdit* lineEdit) {
    if (!lineEdit) {
        return;
    }

    if (lineEdit_) {
        lineEdit_->removeEventFilter(this);
        layout_->removeWidget(lineEdit_);
        lineEdit_->deleteLater();
    }

    lineEdit_ = lineEdit;
    lineEdit_->setParent(this);

    lineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout_->insertWidget(0, lineEdit_);

    lineEdit_->installEventFilter(this);

    connect(lineEdit_, &QLineEdit::returnPressed, this, [this]() {
        emit sentCommandTriggered(lineEdit_->text());
        lineEdit_->clear();
    });
}