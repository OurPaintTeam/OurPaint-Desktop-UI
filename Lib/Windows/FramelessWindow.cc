#include "FramelessWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <windowsx.h>

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
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, style);

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &cornerPref,
        sizeof(cornerPref)
    );
}
#endif


void UI::FramelessWindow::updateWindowCorners() {
#ifdef Q_OS_WIN
    HWND hwnd = (HWND) winId();

    DWM_WINDOW_CORNER_PREFERENCE pref;

    if (isMaximized() || isFullScreen()) {
        pref = DWMWCP_DONOTROUND;
    } else {
        pref = DWMWCP_ROUND;
    }

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &pref,
        sizeof(pref)
    );
#endif
}


bool UI::FramelessWindow::nativeEvent(const QByteArray& eventType,
                                      void *message,
                                      qintptr *result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {

        MSG* msg = static_cast<MSG*>(message);

        switch (msg->message) {

            case WM_NCHITTEST: {
                const LONG borderWidth = 8;

                RECT winrect;
                GetWindowRect(msg->hwnd, &winrect);

                long x = GET_X_LPARAM(msg->lParam);
                long y = GET_Y_LPARAM(msg->lParam);

                bool left   = x < winrect.left + borderWidth;
                bool right  = x > winrect.right - borderWidth;
                bool top    = y < winrect.top + borderWidth;
                bool bottom = y > winrect.bottom - borderWidth;

                if (left && top) {
                    *result = HTTOPLEFT;
                    return true;
                }
                if (right && top) {
                    *result = HTTOPRIGHT;
                    return true;
                }
                if (left && bottom) {
                    *result = HTBOTTOMLEFT;
                    return true;
                }
                if (right && bottom) {
                    *result = HTBOTTOMRIGHT;
                    return true;
                }
                if (left) {
                    *result = HTLEFT;
                    return true;
                }
                if (right) {
                    *result = HTRIGHT;
                    return true;
                }
                if (top) {
                    *result = HTTOP;
                    return true;
                }
                if (bottom) {
                    *result = HTBOTTOM;
                    return true;
                }

                *result = HTCLIENT;
                return true;
            }
        }
    }
#endif

    return QMainWindow::nativeEvent(eventType, message, result);
}


void UI::FramelessWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        updateWindowCorners();
    }
    QMainWindow::changeEvent(event);
}


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
