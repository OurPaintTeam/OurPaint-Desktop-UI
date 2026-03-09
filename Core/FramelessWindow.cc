#include "FramelessWindow.h"

#include <QMouseEvent>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#endif


namespace
{

// Drag area height
constexpr qint32 TITLE_HEIGHT = 32;

// Resize border size
constexpr qint32 RESIZE_BORDER = 8;

#ifdef Q_OS_WIN
constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
constexpr DWORD DWMWCP_ROUND = 2;
#endif

}


FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("frameless"));
    setStyleSheet(QStringLiteral("background:#978897;"));
    resize(960, 540);

#ifdef Q_OS_WIN
    // Enable rounded corners
    const auto hwnd = reinterpret_cast<HWND>(winId());

    constexpr DWORD corner = DWMWCP_ROUND;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner,
        sizeof(corner)
    );
#endif
}


#ifdef Q_OS_WIN
bool FramelessWindow::nativeEvent(const QByteArray&,
                                  void* message,
                                  qintptr* result)
{
    MSG* msg = static_cast<MSG*>(message);

    switch (msg->message)
    {
        case WM_NCCALCSIZE:
        {
            if (msg->wParam)
            {
                *result = 0;
                return true;
            }
            break;
        }

        case WM_NCHITTEST:
        {
            const POINT pt
            {
                GET_X_LPARAM(msg->lParam),
                GET_Y_LPARAM(msg->lParam)
            };

            RECT rect {};
            GetWindowRect(reinterpret_cast<HWND>(winId()), &rect);

            // Top resize
            if (pt.y < rect.top + RESIZE_BORDER)
            {
                if (pt.x < rect.left + RESIZE_BORDER)
                {
                    *result = HTTOPLEFT;
                    return true;
                }

                if (pt.x > rect.right - RESIZE_BORDER)
                {
                    *result = HTTOPRIGHT;
                    return true;
                }

                *result = HTTOP;
                return true;
            }

            // Bottom resize
            if (pt.y > rect.bottom - RESIZE_BORDER)
            {
                if (pt.x < rect.left + RESIZE_BORDER)
                {
                    *result = HTBOTTOMLEFT;
                    return true;
                }

                if (pt.x > rect.right - RESIZE_BORDER)
                {
                    *result = HTBOTTOMRIGHT;
                    return true;
                }

                *result = HTBOTTOM;
                return true;
            }

            if (pt.x < rect.left + RESIZE_BORDER)
            {
                *result = HTLEFT;
                return true;
            }

            if (pt.x > rect.right - RESIZE_BORDER)
            {
                *result = HTRIGHT;
                return true;
            }

            const QPoint global(pt.x, pt.y);
            const QPoint local = mapFromGlobal(global);

            // Allow interaction with children
            if (const QWidget* child = childAt(local);
                child && child != this)
            {
                *result = HTCLIENT;
                return true;
            }

            // Drag zone
            if (local.y() < TITLE_HEIGHT)
            {
                *result = HTCAPTION;
                return true;
            }

            *result = HTCLIENT;
            return true;
        }

        default:
            break;
    }

    return QMainWindow::nativeEvent({}, message, result);
}
#endif


void FramelessWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < TITLE_HEIGHT)
    {
        // Start drag
        m_isDragging = true;

        m_dragOffset =
            event->globalPosition().toPoint() -
            frameGeometry().topLeft();

        event->accept();
    }
}


void FramelessWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging &&
        (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - m_dragOffset);

        event->accept();
    }
}


void FramelessWindow::mouseReleaseEvent(QMouseEvent*)
{
    // Stop drag
    m_isDragging = false;
}