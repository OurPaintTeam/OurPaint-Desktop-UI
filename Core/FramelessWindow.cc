#include "FramelessWindow.h"

#include <QApplication>
#include <QFile>
#include <QMouseEvent>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>

#pragma comment(lib, "dwmapi.lib")

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#endif


namespace {
    // Drag area height
    constexpr qint32 TITLE_HEIGHT = 32;

    // Resize border size
    constexpr qint32 RESIZE_BORDER = 8;

#ifdef Q_OS_WIN
    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
    constexpr DWORD DWMWCP_ROUND = 2;
#endif
} // namespace


UI::FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent) {
    setObjectName(QStringLiteral("FramelessWindow"));
    setWindowTitle("OurPaint");
    setWindowIcon(QIcon(":/Assets/logo/logo2.ico"));

    const auto style = loadStyles({
        ":/Styles/app.qss",
        ":/Styles/buttons.qss",
        ":/Styles/menu.qss",
        ":/Styles/tooltip.qss",
        ":/Styles/window.qss"
    });

    if (style.isEmpty()) {
        qWarning() << "No styles loaded!";
    } else {
        setStyleSheet(style);
    }


    QApplication::setQuitOnLastWindowClosed(true);
    constexpr int sizeW = 960;
    constexpr int sizeY = 540;
    constexpr auto size = QSize(sizeW, sizeY);
    resize(size);


#ifdef Q_OS_WIN
    // Enable rounded corners
    auto* const hwnd = reinterpret_cast<HWND>(winId());

    constexpr DWORD corner = DWMWCP_ROUND;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner,
        sizeof(corner)
    );
#endif
}


void UI::FramelessWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < TITLE_HEIGHT) {
        // Start drag
        isDragging_ = true;
        setCursor(Qt::ClosedHandCursor);

        dragOffset_ =
                event->globalPosition().toPoint() -
                frameGeometry().topLeft();

        event->accept();
    }
}


void UI::FramelessWindow::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging_ &&
        (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - dragOffset_);

        event->accept();
    }
}


void UI::FramelessWindow::mouseReleaseEvent(QMouseEvent*) {
    // Stop drag
    isDragging_ = false;
    setCursor(Qt::ArrowCursor);
}


bool UI::FramelessWindow::nativeEvent(const QByteArray&,
                                      void* message,
                                      qintptr* result) {
#ifdef Q_OS_WIN
    switch (const auto* msg = static_cast<MSG*>(message); msg->message) {
        case WM_NCCALCSIZE: {
            if (msg->wParam) {
                *result = 0;
                return true;
            }
            break;
        }

        case WM_NCHITTEST: {
            const POINT pt
            {
                GET_X_LPARAM(msg->lParam),
                GET_Y_LPARAM(msg->lParam)
            };

            RECT rect{};
            GetWindowRect(reinterpret_cast<HWND>(winId()), &rect);

            // Top resize
            if (pt.y < rect.top + RESIZE_BORDER) {
                if (pt.x < rect.left + RESIZE_BORDER) {
                    *result = HTTOPLEFT;
                    return true;
                }

                if (pt.x > rect.right - RESIZE_BORDER) {
                    *result = HTTOPRIGHT;
                    return true;
                }

                *result = HTTOP;
                return true;
            }

            // Bottom resize
            if (pt.y > rect.bottom - RESIZE_BORDER) {
                if (pt.x < rect.left + RESIZE_BORDER) {
                    *result = HTBOTTOMLEFT;
                    return true;
                }

                if (pt.x > rect.right - RESIZE_BORDER) {
                    *result = HTBOTTOMRIGHT;
                    return true;
                }

                *result = HTBOTTOM;
                return true;
            }

            if (pt.x < rect.left + RESIZE_BORDER) {
                *result = HTLEFT;
                return true;
            }

            if (pt.x > rect.right - RESIZE_BORDER) {
                *result = HTRIGHT;
                return true;
            }

            const QPoint global(pt.x, pt.y);
            const auto local = mapFromGlobal(global);

            // Allow interaction with children
            if (const QWidget* child = childAt(local);
                child && child != this) {
                *result = HTCLIENT;
                return true;
            }

            // Drag zone
            if (local.y() < TITLE_HEIGHT) {
                *result = HTCAPTION;
                return true;
            }

            *result = HTCLIENT;
            return true;
        }

        default:
            break;
    }
#endif
    return QMainWindow::nativeEvent({}, message, result);
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
