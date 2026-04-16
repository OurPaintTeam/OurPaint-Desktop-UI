#include "NavigationWidget.h"

#include <QPushButton>
#include <QVBoxLayout>


UI::NavigationWidget::NavigationWidget(QWidget* parent)
    : QWidget(parent),
      mainLayout_(new QVBoxLayout(this)),
      buttonPlane_(new QWidget(this)),
      layout_(new QVBoxLayout(buttonPlane_)) {
    setAttribute(Qt::WA_TranslucentBackground);

    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);
    mainLayout_->setAlignment(Qt::AlignCenter);

    layout_->setAlignment(Qt::AlignCenter);
    constexpr auto spacing = 10;
    constexpr auto margins = 20;
    layout_->setSpacing(spacing);
    layout_->setContentsMargins(margins, margins, margins, margins);

    startButton_ = new QPushButton("Go to Start Window");
    createFileButton_ = new QPushButton("Create File");
    createProjectButton_ = new QPushButton("Create Project");
    closeButton_ = new QPushButton("Close Application");

    startButton_->setToolTip("Go to Start Window");
    createFileButton_->setToolTip("Create File");
    createProjectButton_->setToolTip("Create Project");
    closeButton_->setToolTip("Close Application");

    buttonPlane_->setObjectName("buttonPlaneNavigationPage");
    setObjectName("NavigationPage");
    startButton_->setObjectName("startButton_");
    createFileButton_->setObjectName("createFileButton_");
    createProjectButton_->setObjectName("createProjectButton_");
    closeButton_->setObjectName("closeButton_");

    layout_->addWidget(startButton_);
    layout_->addWidget(createFileButton_);
    layout_->addWidget(createProjectButton_);
    layout_->addWidget(closeButton_);

    mainLayout_->addWidget(buttonPlane_);

    connect(startButton_, &QPushButton::clicked,
            this, &NavigationWidget::goToStartWindowTriggered);

    connect(createFileButton_, &QPushButton::clicked,
            this, &NavigationWidget::createFileTriggered);

    connect(createProjectButton_, &QPushButton::clicked,
            this, &NavigationWidget::createProjectTriggered);

    connect(closeButton_, &QPushButton::clicked,
            this, &NavigationWidget::closeApplicationTriggered);
}
