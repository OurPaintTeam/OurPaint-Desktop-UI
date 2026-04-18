#include "FramelessWindow.h"

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_BORDER_COLOR 34
#define DWMWA_CAPTION_COLOR 35
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

UI::FramelessWindow::FramelessWindow(QWidget *parent) : QMainWindow(parent) {
  // setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_NoSystemBackground);
  setObjectName(QStringLiteral("FramelessWindow"));
  setWindowTitle("OurPaint");
  setWindowIcon(QIcon(":/Assets/logo/logo2.ico"));

  const auto style = loadStyles({":/Styles/app.qss", ":/Styles/buttons.qss",
                                 ":/Styles/menu.qss", ":/Styles/tooltip.qss",
                                 ":/Styles/window.qss", ":/Styles/text.qss",
                                 ":/Styles/scroll.qss"});

  if (style.isEmpty()) {
    qWarning() << "No styles loaded!";
  } else {
    setStyleSheet(style);
  }

#ifdef Q_OS_WIN
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint); /* отключает заголовок*/
  initWindowForWindowsFrame();
  // initWindowForWindowsWithoutFrame();
#endif

  constexpr int sizeW = 960;
  constexpr int sizeY = 540;
  constexpr auto size = QSize(sizeW, sizeY);
  resize(size);
}

#ifdef Q_OS_WIN
void UI::FramelessWindow::initWindowForWindowsWithoutFrame() const {
  HWND hwnd = (HWND)winId();
  LONG style = GetWindowLong(hwnd, GWL_STYLE);
  style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
  SetWindowLong(hwnd, GWL_STYLE, style);

  DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
  DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref,
                        sizeof(cornerPref));
}

void UI::FramelessWindow::initWindowForWindowsFrame() const {
  // либо  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
  // либо
  HWND hwnd = (HWND)winId();
/*
  LONG style = GetWindowLong(hwnd, GWL_STYLE);
  style &= ~(WS_CAPTION | WS_SYSMENU);
  SetWindowLong(hwnd, GWL_STYLE, style);*/

  COLORREF color = RGB(31, 52, 56);
 // COLORREF color = RGB(73, 72, 80); /* как у титле бар */
  DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

  COLORREF borderColor = RGB(73, 72, 80);
  DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderColor,
                        sizeof(borderColor));
}

void UI::FramelessWindow::updateWindowCorners() const {
  HWND hwnd = (HWND)winId();

  DWM_WINDOW_CORNER_PREFERENCE pref;

  if (isMaximized() || isFullScreen()) {
    pref = DWMWCP_DONOTROUND;
  } else {
    pref = DWMWCP_ROUND;
  }

  DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref,
                        sizeof(pref));
}
#endif

bool UI::FramelessWindow::nativeEvent(const QByteArray &eventType,
                                      void *message, qintptr *result) {
  /*
   * Если initWindowForWindowsWithoutFrame()
   */
  /*
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

                bool left = x < winrect.left + borderWidth;
                bool right = x > winrect.right - borderWidth;
                bool top = y < winrect.top + borderWidth;
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
            default:;
        }
    }
#endif
*/
  return QMainWindow::nativeEvent(eventType, message, result);
}

void UI::FramelessWindow::changeEvent(QEvent *event) {
#ifdef Q_OS_WIN
  if (event->type() == QEvent::WindowStateChange) {
    /*
     * Если initWindowForWindowsWithoutFrame()
     */
    //   updateWindowCorners();
  }
#endif
  QMainWindow::changeEvent(event);
}

QString UI::FramelessWindow::loadStyle(const QString &path) {
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

QString UI::FramelessWindow::loadStyles(const QStringList &files) {
  QString style;
  for (const auto &file : files) {
    style += loadStyle(file);
  }
  return style;
}

void UI::FramelessWindow::showEvent(QShowEvent *event) {
  if (isStart) {
    QRect screenGeometry =
        QGuiApplication::primaryScreen()->availableGeometry();
    move(screenGeometry.center() - rect().center());
    isStart = false;
  }
  QMainWindow::showEvent(event);
}