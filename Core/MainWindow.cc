#include "MainWindow.h"

#include "StartPage.h"
#include "ProjectPage.h"
#include "TabPage.h"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QCloseEvent>

namespace
{
constexpr qint32 LAYOUT_MARGIN = 0;
constexpr qint32 LAYOUT_SPACING = 0;
}


MainWindow::MainWindow(QWidget* parent)
    : FramelessWindow(parent)
{
    setObjectName(QStringLiteral("mainwindow"));
    initUI();
}


MainWindow::~MainWindow()
{
    // Ensure detached tabs are closed
    for (TabPage* tab : m_tabs)
    {
        if (tab != nullptr)
        {
            tab->close();
        }
    }

    m_tabs.clear();
}


void MainWindow::initUI()
{
    // Root widget
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName(QStringLiteral("mainContent"));

    setCentralWidget(m_contentWidget);

    // Root layout
    m_rootLayout = new QVBoxLayout(m_contentWidget);
    m_rootLayout->setObjectName(QStringLiteral("mainLayout"));
    m_rootLayout->setContentsMargins(
        LAYOUT_MARGIN,
        LAYOUT_MARGIN,
        LAYOUT_MARGIN,
        LAYOUT_MARGIN
    );
    m_rootLayout->setSpacing(LAYOUT_SPACING);

    // Page stack
    m_pageStack = new QStackedWidget(m_contentWidget);
    m_pageStack->setObjectName(QStringLiteral("appStack"));

    m_rootLayout->addWidget(m_pageStack);

    // Pages
    m_startPage = new StartPage(m_pageStack);
    m_startPage->setObjectName(QStringLiteral("startPage"));

    m_projectPage = new ProjectPage(m_pageStack);
    m_projectPage->setObjectName(QStringLiteral("projectPage"));

    // Forward tab request
    connect(
        m_projectPage,
        &ProjectPage::requestOpenTabPage,
        this,
        &MainWindow::openTabPage
    );

    m_pageStack->addWidget(m_startPage);
    m_pageStack->addWidget(m_projectPage);

    m_pageStack->setCurrentWidget(m_projectPage);
}


void MainWindow::openTabPage(const QString& tabName)
{
    // Create detached tab window
    TabPage* tab = new TabPage(nullptr);

    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->setObjectName(QStringLiteral("tabPage"));
    tab->setWindowTitle(tabName);

    m_tabs.append(tab);

    // Remove pointer when destroyed
    connect(
        tab,
        &QObject::destroyed,
        this,
        [this, tab]()
        {
            m_tabs.removeAll(tab);
        }
    );

    tab->show();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    // Close all detached tabs
    for (TabPage* tab : m_tabs)
    {
        if (tab != nullptr)
        {
            tab->close();
        }
    }

    m_tabs.clear();

    FramelessWindow::closeEvent(event);
}