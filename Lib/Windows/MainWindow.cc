#include "MainWindow.h"
#include "StartPage.h"
#include "EditorPage.h"
#include "NotificationContainer.h"
#include <QVBoxLayout>

UI::MainWindow::MainWindow(QWidget* parent)
    : FramelessWindow(parent) {
    initUI();
}

void UI::MainWindow::initUI() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    stack_ = new QStackedWidget(central);
    layout->addWidget(stack_);

    startPage_ = new StartPage(stack_);
    editorPage_ = new EditorPage(stack_);

    stack_->addWidget(startPage_);
    stack_->addWidget(editorPage_);

    notificationManager_ = new NotificationContainer(this);
}

void UI::MainWindow::addNotification(const QString& text) const {
    if (notificationManager_) {
        notificationManager_->addNotification(text);
    }
}