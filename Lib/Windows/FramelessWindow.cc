#include "FramelessWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <dwmapi.h>


#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

enum DWM_WINDOW_CORNER_PREFERENCE {
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
};

#endif


#include <QApplication>
#include <QFile>
#include <QMouseEvent>


UI::FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setObjectName(QStringLiteral("FramelessWindow"));
    setWindowTitle("OurPaint");
    setWindowIcon(QIcon(":/Assets/logo/logo2.ico"));

    const auto style = loadStyles({
        ":/Styles/app.qss",
        ":/Styles/buttons.qss",
        ":/Styles/menu.qss",
        ":/Styles/tooltip.qss",
        ":/Styles/window.qss",
        ":/Styles/text.qss",
        ":/Styles/scroll.qss"
    });

    if (style.isEmpty()) {
        qWarning() << "No styles loaded!";
    } else {
        setStyleSheet(style);
    }

#ifdef Q_OS_WIN
    initWindowForWindows();
#endif

    QApplication::setQuitOnLastWindowClosed(true);
    constexpr int sizeW = 960;
    constexpr int sizeY = 540;
    constexpr auto size = QSize(sizeW, sizeY);
    resize(size);
}

#ifdef Q_OS_WIN
void UI::FramelessWindow::initWindowForWindows() const {
    HWND hwnd = (HWND) winId();
    LONG winStyle = GetWindowLong(hwnd, GWL_STYLE);
    winStyle &= ~WS_CAPTION;
    winStyle |= WS_THICKFRAME;
    winStyle &= ~WS_SYSMENU;

    SetWindowLong(hwnd, GWL_STYLE, winStyle);

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &cornerPref,
        sizeof(cornerPref)
    );
}


bool UI::FramelessWindow::nativeEvent(const QByteArray&,
                                      void *message,
                                      qintptr *result) {
    return QMainWindow::nativeEvent({}, message, result);
}
#endif


QString UI::FramelessWindow::loadStyle(const QString& path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "Resource not found:" << path;
        return "";
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open style file:" << path;
        return "";
    }

    return QString::fromUtf8(file.readAll());
}


QString UI::FramelessWindow::loadStyles(const QStringList& files) {
    QString style;
    for (const auto& file: files) {
        style += loadStyle(file);
    }
    return style;
}
