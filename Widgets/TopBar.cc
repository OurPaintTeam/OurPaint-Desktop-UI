#include "TopBar.h"

#include <QApplication>

#include "HideOpenPanelButton.h"


UI::TopBar::TopBar(QWidget* parent)
    : QWidget(parent), layout_(new QHBoxLayout(this)) {
    constexpr auto buttonSizeH = 32;
    setFixedHeight(buttonSizeH);
    setAttribute(Qt::WA_StyledBackground, true);

    setupStyle();

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    layout_->addStretch();

    setupWindowButtons();
    setObjectName(QStringLiteral("topBar"));
}


QPushButton* UI::TopBar::createWindowButton() {
    auto* button = new QPushButton(this);
    constexpr auto buttonSize = QSize(32, 32);
    button->setFixedSize(buttonSize);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}


UI::HideOpenPanelButton* UI::TopBar::createPanelButton() {
    auto* button = new UI::HideOpenPanelButton(this);
    constexpr auto buttonSize = QSize(32, 32);
    button->setFixedSize(buttonSize);
    return button;
}


QHBoxLayout* UI::TopBar::getLayout() const {
    return layout_;
}


void UI::TopBar::setupStyle() {
    setObjectName("TopBar");

    setStyleSheet(R"(
    #TopBar {
        background: #494850;
        border-bottom: 1px solid #3e3d42;
    }
)");
}


void UI::TopBar::setupWindowButtons() {
    minButton_ = createWindowButton();
    const QIcon minIcon("../Assets/windowButtons/Minimize.png");
    minButton_->setIcon(minIcon);
    minButton_->setToolTip("Minimize window");
    minButton_->setObjectName("minApplication");

    maxButton_ = createWindowButton();
    const QIcon maxIcon("../Assets/windowButtons/Maximize.png");
    maxButton_->setIcon(maxIcon);
    maxButton_->setToolTip("Maximize window");
    maxButton_->setObjectName("maxApplication");

    closeButton_ = createWindowButton();
    const QIcon closeIcon("../Assets/windowButtons/Close.png");
    closeButton_->setIcon(closeIcon);
    closeButton_->setToolTip("Close window");
    closeButton_->setObjectName("closeButton");

    layout_->addWidget(minButton_);
    layout_->addWidget(maxButton_);
    layout_->addWidget(closeButton_);

    connect(minButton_, &QPushButton::clicked, []() {
        if (auto* window = QApplication::activeWindow()) {
            window->showMinimized();
        }
    });

    connect(maxButton_, &QPushButton::clicked, []() {
        if (auto* window = QApplication::activeWindow()) {
            window->isMaximized() ? window->showNormal() : window->showMaximized();
        }
    });

    connect(closeButton_, &QPushButton::clicked, []() {
        if (auto* window = QApplication::activeWindow()) {
            window->close();
        }
    });
}
