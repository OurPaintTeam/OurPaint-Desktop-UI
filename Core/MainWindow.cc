#include "MainWindow.h"

#include "ProjectPage.h"
#include "StartPage.h"
#include "TabPage.h"

#include <QCloseEvent>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {
    constexpr qint32 LAYOUT_MARGIN = 0;
    constexpr qint32 LAYOUT_SPACING = 0;
} // namespace

using namespace UI;


MainWindow::MainWindow(QWidget* parent)
    : FramelessWindow(parent) {
    setObjectName(QStringLiteral("mainwindow"));
    initUI();
}


MainWindow::~MainWindow() {
    // Ensure detached tabs are closed
    for (TabPage* tab: tabs_) {
        if (tab != nullptr) {
            tab->close();
        }
    }

    tabs_.clear();
}


void MainWindow::openTabPage(const QString& tabName) {
    // Create detached tab window
    auto* tab = new TabPage(nullptr);

    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->setObjectName(QStringLiteral("tabPage"));
    tab->setWindowTitle(tabName);

    tabs_.append(tab);

    // Remove pointer when destroyed
    connect(tab, &QObject::destroyed, this,
            [this, tab](QObject* /*obj*/) {
                tabs_.removeAll(tab);
            }
    );

    tab->show();
}


void MainWindow::initUI() {
    // Root widget
    contentWidget_ = new QWidget(this);
    contentWidget_->setObjectName(QStringLiteral("mainContent"));

    setCentralWidget(contentWidget_);

    // Root layout
    rootLayout_ = new QVBoxLayout(contentWidget_);
    rootLayout_->setObjectName(QStringLiteral("mainLayout"));
    rootLayout_->setContentsMargins(
        LAYOUT_MARGIN,
        LAYOUT_MARGIN,
        LAYOUT_MARGIN,
        LAYOUT_MARGIN
    );
    rootLayout_->setSpacing(LAYOUT_SPACING);

    // Page stack
    pageStack_ = new QStackedWidget(contentWidget_);
    pageStack_->setObjectName(QStringLiteral("appStack"));

    rootLayout_->addWidget(pageStack_);

    // Pages
    startPage_ = new StartPage(pageStack_);
    startPage_->setObjectName(QStringLiteral("startPage"));

    projectPage_ = new ProjectPage(pageStack_);
    projectPage_->setObjectName(QStringLiteral("projectPage"));

    // Forward tab request
    connect(
        projectPage_,
        &ProjectPage::requestOpenTabPage,
        this,
        &MainWindow::openTabPage
    );

    pageStack_->addWidget(startPage_);
    pageStack_->addWidget(projectPage_);

    pageStack_->setCurrentWidget(projectPage_);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    // Close all detached tabs
    for (auto* tab: tabs_) {
        if (tab != nullptr) {
            tab->close();
        }
    }

    tabs_.clear();

    FramelessWindow::closeEvent(event);
}
