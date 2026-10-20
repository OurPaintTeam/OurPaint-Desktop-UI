namespace OurPaint { } // OurPaint library namespace

#ifdef _WIN32
#include "Platform.h"
#include "App.h"
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <GLFW/glfw3native.h>
#include <algorithm>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dwmapi.lib")

static HWND          g_hwnd             = nullptr;
static GLFWwindow*   g_glfwWindow       = nullptr;
static WNDPROC       g_originalWndProc  = nullptr;
static bool          g_maximizeHovered  = false;
static bool          g_maximizePressed  = false;

// ---------- DWM ----------
static void ApplyDwmStyle(HWND hwnd) {
    const int cornerPreferenceRound = 2;
    DwmSetWindowAttribute(hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(33),
        &cornerPreferenceRound, sizeof(cornerPreferenceRound));

    const COLORREF borderColor = RGB(45, 48, 55);
    DwmSetWindowAttribute(hwnd,
        static_cast<DWMWINDOWATTRIBUTE>(34),
        &borderColor, sizeof(borderColor));
}

// ---------- cursors ----------
static bool ApplyResizeCursor(LRESULT hit) {
    switch (hit) {
    case HTLEFT: case HTRIGHT:
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE)); return true;
    case HTTOP: case HTBOTTOM:
        SetCursor(LoadCursor(nullptr, IDC_SIZENS)); return true;
    case HTTOPLEFT: case HTBOTTOMRIGHT:
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE)); return true;
    case HTTOPRIGHT: case HTBOTTOMLEFT:
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW)); return true;
    }
    return false;
}

// ---------- hit test ----------
static LRESULT HitTestWindow(HWND hwnd, LPARAM lParam) {
    const POINT screenPoint{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

    RECT windowRect{};
    GetWindowRect(hwnd, &windowRect);

    const int outerWidth  = windowRect.right  - windowRect.left;
    const int outerHeight = windowRect.bottom - windowRect.top;
    if (outerWidth <= 0 || outerHeight <= 0) return HTCLIENT;

    const int px = screenPoint.x - windowRect.left;
    const int py = screenPoint.y - windowRect.top;

    if (!IsZoomed(hwnd)) {
        const int borderX = std::max(
            6, GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
        const int borderY = std::max(
            6, GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

        const bool left   = px < borderX;
        const bool right  = px >= outerWidth - borderX;
        const bool top    = py < borderY;
        const bool bottom = py >= outerHeight - borderY;

        if (top && left)     return HTTOPLEFT;
        if (top && right)    return HTTOPRIGHT;
        if (bottom && left)  return HTBOTTOMLEFT;
        if (bottom && right) return HTBOTTOMRIGHT;
        if (left)            return HTLEFT;
        if (right)           return HTRIGHT;
        if (top)             return HTTOP;
        if (bottom)          return HTBOTTOM;
    }

    POINT clientPoint = screenPoint;
    ScreenToClient(hwnd, &clientPoint);

    RECT clientRect{};
    GetClientRect(hwnd, &clientRect);

    int glfwWidth = 0, glfwHeight = 0;
    if (g_glfwWindow) glfwGetWindowSize(g_glfwWindow, &glfwWidth, &glfwHeight);

    const int clientPixelWidth  = clientRect.right  - clientRect.left;
    const int clientPixelHeight = clientRect.bottom - clientRect.top;

    const float logicalX = (clientPixelWidth > 0 && glfwWidth > 0)
        ? static_cast<float>(clientPoint.x) * glfwWidth / clientPixelWidth
        : static_cast<float>(clientPoint.x);

    const float logicalY = (clientPixelHeight > 0 && glfwHeight > 0)
        ? static_cast<float>(clientPoint.y) * glfwHeight / clientPixelHeight
        : static_cast<float>(clientPoint.y);

    if (glfwWidth > 0)
        UpdateCaptionHitRects(static_cast<float>(glfwWidth));

    if (Contains(g_maximizeRect, logicalX, logicalY))
        return HTMAXBUTTON;

    if (Contains(g_minimizeRect, logicalX, logicalY) ||
        Contains(g_closeRect,    logicalX, logicalY))
        return HTCLIENT;

    // Project button in the titlebar must receive client mouse input.
    const ButtonHitRect projectButton{104.0f, 9.0f, 88.0f, 30.0f};
    if (Contains(projectButton, logicalX, logicalY))
        return HTCLIENT;

    if (logicalY >= 0.0f && logicalY < TITLEBAR_HEIGHT)
        return HTCAPTION;

    return HTCLIENT;
}

// ---------- WndProc ----------
static LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCMOUSEMOVE && wParam == HTMAXBUTTON) {
        g_maximizeHovered = true;
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
    }
    else if (message == WM_NCMOUSEMOVE) {
        g_maximizePressed = false;
    }
    else if (message == WM_NCMOUSELEAVE) {
        g_maximizeHovered = false;
        g_maximizePressed = false;
    }
    else if (message == WM_NCLBUTTONDOWN && wParam == HTMAXBUTTON) {
        g_maximizePressed = true;
        return 0;
    }
    else if (message == WM_NCLBUTTONUP && wParam == HTMAXBUTTON) {
        g_maximizePressed = false;
        if (IsZoomed(hwnd))
            PostMessageW(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        else
            PostMessageW(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        return 0;
    }

    if (message == WM_NCHITTEST) {
        const LRESULT hit = HitTestWindow(hwnd, lParam);
        ApplyResizeCursor(hit);
        return hit;
    }

    switch (message) {
    case WM_GETMINMAXINFO: {
        if (g_originalWndProc)
            CallWindowProcW(g_originalWndProc, hwnd, message, wParam, lParam);
        else
            DefWindowProcW(hwnd, message, wParam, lParam);

        auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
        HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{ sizeof(MONITORINFO) };
        if (GetMonitorInfoW(hMon, &mi)) {
            mmi->ptMaxPosition.x = mi.rcWork.left   - mi.rcMonitor.left;
            mmi->ptMaxPosition.y = mi.rcWork.top    - mi.rcMonitor.top;
            mmi->ptMaxSize.x     = mi.rcWork.right  - mi.rcWork.left;
            mmi->ptMaxSize.y     = mi.rcWork.bottom - mi.rcWork.top;
        }
        return 0;
    }

    case WM_NCCALCSIZE:
        if (wParam == TRUE) {
            if (IsZoomed(hwnd)) {
                auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                MONITORINFO mi{ sizeof(MONITORINFO) };
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (monitor && GetMonitorInfoW(monitor, &mi))
                    params->rgrc[0] = mi.rcWork;
            }
            return 0;
        }
        break;

    case WM_NCMOUSEMOVE:
    case WM_NCMOUSELEAVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP: {
        LRESULT dwmResult = 0;
        if (DwmDefWindowProc(hwnd, message, wParam, lParam, &dwmResult))
            return dwmResult;
        break;
    }

    case WM_SETCURSOR:
        if (reinterpret_cast<HWND>(wParam) == hwnd) {
            POINT pt;
            GetCursorPos(&pt);
            const LRESULT hit = HitTestWindow(hwnd, MAKELPARAM(pt.x, pt.y));
            if (ApplyResizeCursor(hit)) return TRUE;
        }
        break;

    case WM_ENTERSIZEMOVE:
        if (g_glfwWindow) glfwSwapInterval(0);
        break;

    case WM_EXITSIZEMOVE:
        if (g_glfwWindow) {
            glfwSwapInterval(1);
            if (!AppIsRendering()) RenderFrame(g_glfwWindow);
        }
        break;

    case WM_SIZE: {
        LRESULT result = g_originalWndProc
            ? CallWindowProcW(g_originalWndProc, hwnd, message, wParam, lParam)
            : DefWindowProcW(hwnd, message, wParam, lParam);

        if (g_glfwWindow && !AppIsRendering() && wParam != SIZE_MINIMIZED)
            RenderFrame(g_glfwWindow);
        return result;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DWMCOMPOSITIONCHANGED:
        ApplyDwmStyle(hwnd);
        break;
    }

    if (g_originalWndProc)
        return CallWindowProcW(g_originalWndProc, hwnd, message, wParam, lParam);
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// ---------- Platform API ----------
void Platform::init(GLFWwindow* window) {
    g_glfwWindow = window;
    g_hwnd = glfwGetWin32Window(window);

    SetLastError(0);
    const LONG_PTR oldProc = SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(CustomWndProc));
    if (oldProc == 0 && GetLastError() != 0) return;
    g_originalWndProc = reinterpret_cast<WNDPROC>(oldProc);

    LONG_PTR style = GetWindowLongPtrW(g_hwnd, GWL_STYLE);
    style &= ~static_cast<LONG_PTR>(WS_CAPTION);
    style |= WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    SetWindowLongPtrW(g_hwnd, GWL_STYLE, style);

    SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    ApplyDwmStyle(g_hwnd);
}

void Platform::shutdown() {
    if (g_originalWndProc)
        SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(g_originalWndProc));
    g_originalWndProc = nullptr;
    g_hwnd = nullptr;
    g_glfwWindow = nullptr;
}

HitTest Platform::hitTest(GLFWwindow*, double, double) { return HitTest::Client; }
void Platform::updateDrag(GLFWwindow*) {}

void Platform::minimize(GLFWwindow*) {
    SendMessageW(g_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}
void Platform::maximizeRestore(GLFWwindow*) {
    PostMessageW(g_hwnd, WM_SYSCOMMAND,
        IsZoomed(g_hwnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
}
void Platform::close(GLFWwindow*) {
    PostMessageW(g_hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
}
bool Platform::isMaximized(GLFWwindow*) { return IsZoomed(g_hwnd) != FALSE; }
void Platform::applyWindowStyle(GLFWwindow*) { ApplyDwmStyle(g_hwnd); }
bool Platform::maximizeHovered() { return g_maximizeHovered; }
bool Platform::maximizePressed() { return g_maximizePressed; }

#endif // _WIN32