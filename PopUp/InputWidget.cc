#include "InputWidget.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>


UI::InputWidget::InputWidget(const QString& promptText, QWidget* parent)
    : QWidget(parent),
      label_(new QLabel(promptText, this)),
      lineEdit_(new QLineEdit(this)),
      closeButton_(new QPushButton(this)),
      checkBox_(new QCheckBox(this)),
      buttonLayout_(new QHBoxLayout()),
      mainLayout_(new QVBoxLayout(this)),
      headerLayout_(new QHBoxLayout()) {
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setObjectName("InputWidget");

    constexpr auto w = 270;
    setMinimumWidth(w);

    constexpr auto m = 270;
    mainLayout_->setContentsMargins(m, m, m, m);
    constexpr auto s = 8;
    mainLayout_->setSpacing(s);

    headerLayout_->setContentsMargins(0, 0, 0, 0);

    label_->setObjectName("InputWidgetLabel");

    auto f = label_->font();
    f.setBold(true);
    label_->setFont(f);

    closeButton_->setObjectName("InputWidgetCloseButton");
    constexpr auto clSize = QSize(24, 24);
    closeButton_->setFixedSize(clSize);
    closeButton_->setIcon(QIcon(":/Assets/icons/window/close.png"));
    constexpr auto clISize = QSize(16, 16);
    closeButton_->setIconSize(clISize);

    headerLayout_->addWidget(label_);
    headerLayout_->addStretch();
    headerLayout_->addWidget(closeButton_);

    lineEdit_->setObjectName("InputWidgetLineEdit");
    constexpr auto h = 30;
    lineEdit_->setFixedHeight(h);
    lineEdit_->setFocus();

    checkBox_->setObjectName("InputWidgetCheckBox");
    checkBox_->hide();

    okButton_ = new QPushButton("OK", this);
    okButton_->setObjectName("InputWidgetOkButton");
    constexpr auto h1 = 26;
    okButton_->setFixedHeight(h1);

    buttonLayout_->addStretch();
    buttonLayout_->addWidget(okButton_);
    buttonLayout_->addStretch();

    mainLayout_->addLayout(headerLayout_);
    mainLayout_->addWidget(lineEdit_);
    mainLayout_->addWidget(checkBox_);
    mainLayout_->addLayout(buttonLayout_);

    connect(okButton_, &QPushButton::clicked,
            this, &InputWidget::onOkClickedSlot);

    connect(lineEdit_, &QLineEdit::returnPressed,
            this, &InputWidget::onOkClickedSlot);

    connect(closeButton_, &QPushButton::clicked,
            this, &QWidget::close);

    adjustSize();
}


void UI::InputWidget::setCheckBoxQuestion(const QString& question) {
    checkBox_->setText(question);
    checkBox_->show();
    adjustSize();
}


bool UI::InputWidget::isCheckBoxChecked() const {
    return checkBox_->isChecked();
}


void UI::InputWidget::onOkClickedSlot() {
    emit inputEnteredTriggered(lineEdit_->text());
    close();
}


void UI::InputWidget::mousePressEvent(QMouseEvent* event) {
    if (constexpr auto x = 35; event->button() == Qt::LeftButton && event->pos().y() <x) {
        dragging_ = true;
        dragPosition_ = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}


void UI::InputWidget::mouseMoveEvent(QMouseEvent* event) {
    if (dragging_ && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - dragPosition_);
    }
}


void UI::InputWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging_ = false;
    }
}


void UI::InputWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect();
    constexpr auto x = 0.5;
    constexpr auto y = -0.5;
    rect.adjust(x, x, y, y);

    painter.setBrush(backgroundColor_);
    painter.setPen(QPen(borderColor_, 1));

    constexpr auto r = 8;
    painter.drawRoundedRect(rect, r, r);
}
