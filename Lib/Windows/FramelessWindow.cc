#include "FramelessWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_BORDER_COLOR 34
#define DWMWA_CAPTION_COLOR 35
#endif
#endif


#include <QApplication>
#include <QFile>
#include <QMouseEvent>


UI::FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent) {
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

    COLORREF color = RGB(73, 72, 80);

    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

    COLORREF borderColor = RGB(73, 72, 80);

    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(borderColor));

    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER);
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
