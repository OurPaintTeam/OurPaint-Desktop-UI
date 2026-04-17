#include "TopBar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QWindow>


UI::TopBar::TopBar(QWidget *parent)
    : QWidget(parent),
      mainLayout_(new QHBoxLayout(this)) {
    constexpr auto size = 32;
    setFixedHeight(size);
    setAttribute(Qt::WA_StyledBackground, true);

    setupLayouts();
    setupWindowButtons();
    setObjectName("TopBar");

    setCursor(Qt::OpenHandCursor);
}


void UI::TopBar::addLeftWidget(QWidget *widget) const {
    leftLayout_->addWidget(widget);
    widget->setCursor(Qt::PointingHandCursor);
}


void UI::TopBar::addCenterWidget(QWidget *widget) const {
    centerLayout_->addWidget(widget);
    widget->setCursor(Qt::PointingHandCursor);
}


QPushButton* UI::TopBar::createWindowButton() {
    auto *button = new QPushButton(this);
    button->setCursor(Qt::PointingHandCursor);
    constexpr auto buttonSize = QSize(32, 32);
    button->setFixedSize(buttonSize);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}


QPushButton* UI::TopBar::createWindowButton(const QString& text) {
    auto *button = new QPushButton(this);
    button->setCursor(Qt::PointingHandCursor);
    button->setToolTip(text);
    button->setObjectName(text);
    button->setText(text);
    constexpr auto buttonSize = QSize(32, 32);
    button->setFixedSize(buttonSize);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}


QPushButton* UI::TopBar::createWindowButton(const QIcon& icon) {
    auto *button = new QPushButton(this);
    button->setCursor(Qt::PointingHandCursor);
    button->setIcon(icon);
    constexpr auto buttonSize = QSize(32, 32);
    button->setFixedSize(buttonSize);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}


void UI::TopBar::mousePressEvent(QMouseEvent *event) {
#ifdef Q_OS_WIN
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
        window()->windowHandle()->startSystemMove();
        event->accept();
        return;
    }
#endif

    QWidget::mousePressEvent(event);
}


void UI::TopBar::mouseReleaseEvent(QMouseEvent *event) {
    setCursor(Qt::OpenHandCursor);
    QWidget::mouseReleaseEvent(event);
}


void UI::TopBar::setupLayouts() {
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    leftLayout_ = new QHBoxLayout();
    leftLayout_->setSpacing(0);
    leftLayout_->setContentsMargins(0, 0, 0, 0);

    leftContainer_ = new QWidget(this);
    leftContainer_->setLayout(leftLayout_);
    mainLayout_->addWidget(leftContainer_, 0, Qt::AlignLeft);

    rightCombinedLayout_ = new QHBoxLayout();
    rightCombinedLayout_->setSpacing(0);
    rightCombinedLayout_->setContentsMargins(0, 0, 0, 0);

    centerLayout_ = new QHBoxLayout();
    centerLayout_->setSpacing(0);
    centerLayout_->setContentsMargins(0, 0, 0, 0);
    centerContainer_ = new QWidget(this);
    centerContainer_->setLayout(centerLayout_);
    rightCombinedLayout_->addWidget(centerContainer_);

    rightLayout_ = new QHBoxLayout();
    rightLayout_->setSpacing(0);
    rightLayout_->setContentsMargins(0, 0, 0, 0);
    rightContainer_ = new QWidget(this);
    rightContainer_->setLayout(rightLayout_);
    rightCombinedLayout_->addWidget(rightContainer_);

    combinedWidget_ = new QWidget(this);
    combinedWidget_->setLayout(rightCombinedLayout_);
    mainLayout_->addWidget(combinedWidget_, 0, Qt::AlignRight);
}


void UI::TopBar::setupWindowButtons() {
    minButton_ = createWindowButton();
    minButton_->setToolTip("Minimize window");
    minButton_->setObjectName("minApplication");

    maxButton_ = createWindowButton();
    maxButton_->setToolTip("Maximize window");
    maxButton_->setObjectName("MaxApplication");

    closeButton_ = createWindowButton();
    closeButton_->setObjectName("CloseApplication");

    minButton_->setIcon(QIcon(":/Assets/icons/window/minimize.png"));
    maxButton_->setIcon(QIcon(":/Assets/icons/window/maximize.png"));
    closeButton_->setIcon(QIcon(":/Assets/icons/window/close.png"));

    minButton_->setToolTip("Minimize");
    maxButton_->setToolTip("Maximize");
    closeButton_->setToolTip("Close");

    rightLayout_->addWidget(minButton_);
    rightLayout_->addWidget(maxButton_);
    rightLayout_->addWidget(closeButton_);

    connect(minButton_, &QPushButton::clicked, []() {
        if (auto *window = QApplication::activeWindow()) {
            window->showMinimized();
        }
    });
    connect(maxButton_, &QPushButton::clicked, []() {
        if (auto *window = QApplication::activeWindow()) {
            window->isMaximized() ? window->showNormal() : window->showMaximized();
        }
    });
    connect(closeButton_, &QPushButton::clicked, []() {
        if (auto *window = QApplication::activeWindow()) {
            window->close();
        }
    });
}