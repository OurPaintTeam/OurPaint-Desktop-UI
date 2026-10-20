namespace OurPaint { } // OurPaint library namespace

#include "Platform.h"
#include "App.h"
#include <algorithm>
#include <cmath>

#ifdef __linux__
#include <GLFW/glfw3.h>
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
#include <GLFW/glfw3native.h>
#endif

// ---------- глобальные состояния для drag/resize ----------
static HitTest g_dragMode        = HitTest::Client;
static bool    g_dragActive      = false;
static double  g_startCursorX    = 0.0;
static double  g_startCursorY    = 0.0;
static int     g_startWinX       = 0;
static int     g_startWinY       = 0;
static int     g_startWinW       = 0;
static int     g_startWinH       = 0;
static int     g_prevMouseState  = GLFW_RELEASE;

static GLFWcursor* g_curHR = nullptr;
static GLFWcursor* g_curVR = nullptr;
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
static GLFWcursor* g_curNWSE = nullptr;
static GLFWcursor* g_curNESW = nullptr;
#endif

// ---------- проверка, что мы под Wayland ----------
static bool IsWayland() {
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    return glfwGetPlatform() == GLFW_PLATFORM_WAYLAND;
#else
    return false;
#endif
}

// ---------- хит-тест ----------
HitTest Platform::hitTest(GLFWwindow* window, double cx, double cy) {
    if (IsWayland()) return HitTest::Client; // на Wayland не используется

    int ww = 0, wh = 0;
    glfwGetWindowSize(window, &ww, &wh);
    const bool maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) != 0;

    if (!maximized) {
        constexpr double edge = 6.0;
        const bool left   = cx < edge;
        const bool right  = cx >= ww - edge;
        const bool top    = cy < edge;
        const bool bottom = cy >= wh - edge;

        if (top && left)     return HitTest::CornerTL;
        if (top && right)    return HitTest::CornerTR;
        if (bottom && left)  return HitTest::CornerBL;
        if (bottom && right) return HitTest::CornerBR;
        if (left)            return HitTest::EdgeL;
        if (right)           return HitTest::EdgeR;
        if (top)             return HitTest::EdgeT;
        if (bottom)          return HitTest::EdgeB;
    }

    if (cy < TITLEBAR_HEIGHT) {
        if (Contains(g_maximizeRect, (float)cx, (float)cy)) return HitTest::MaxBtn;
        if (Contains(g_minimizeRect, (float)cx, (float)cy)) return HitTest::MinBtn;
        if (Contains(g_closeRect,    (float)cx, (float)cy)) return HitTest::CloseBtn;

        // Project button in the titlebar must receive client mouse input.
        const ButtonHitRect projectButton{104.0f, 9.0f, 88.0f, 30.0f};
        if (Contains(projectButton, (float)cx, (float)cy)) return HitTest::Client;
        return HitTest::TitleBar;
    }
    return HitTest::Client;
}

// ---------- курсор ----------
static void SetCursorForHit(GLFWwindow* w, HitTest h) {
    GLFWcursor* c = nullptr;
    switch (h) {
    case HitTest::EdgeL: case HitTest::EdgeR: c = g_curHR; break;
    case HitTest::EdgeT: case HitTest::EdgeB: c = g_curVR; break;
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    case HitTest::CornerTL: case HitTest::CornerBR: c = g_curNWSE; break;
    case HitTest::CornerTR: case HitTest::CornerBL: c = g_curNESW; break;
#else
    case HitTest::CornerTL: case HitTest::CornerTR:
    case HitTest::CornerBL: case HitTest::CornerBR: c = g_curHR; break;
#endif
    default: return;
    }
    glfwSetCursor(w, c);
}

// ---------- drag / resize ----------
void Platform::updateDrag(GLFWwindow* window) {
    if (IsWayland()) return;

    const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    double cx = 0.0, cy = 0.0;
    glfwGetCursorPos(window, &cx, &cy);
    int ww = 0, wh = 0;
    glfwGetWindowSize(window, &ww, &wh);
    int wx = 0, wy = 0;
    glfwGetWindowPos(window, &wx, &wy);
    const double screenX = wx + cx;
    const double screenY = wy + cy;

    if (state == GLFW_PRESS && g_prevMouseState == GLFW_RELEASE) {
        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == 0) {
            HitTest hit = hitTest(window, cx, cy);
            if (hit == HitTest::TitleBar ||
                hit == HitTest::EdgeL || hit == HitTest::EdgeR ||
                hit == HitTest::EdgeT || hit == HitTest::EdgeB ||
                hit == HitTest::CornerTL || hit == HitTest::CornerTR ||
                hit == HitTest::CornerBL || hit == HitTest::CornerBR) {
                g_dragMode     = hit;
                g_dragActive   = true;
                g_startCursorX = screenX;
                g_startCursorY = screenY;
                g_startWinX    = wx;
                g_startWinY    = wy;
                g_startWinW    = ww;
                g_startWinH    = wh;
            }
        }
    } else if (state == GLFW_RELEASE) {
        g_dragActive = false;
    }

    if (g_dragActive) {
        const int dx = (int)(screenX - g_startCursorX);
        const int dy = (int)(screenY - g_startCursorY);
        constexpr int MIN_W = 400, MIN_H = 300;

        switch (g_dragMode) {
        case HitTest::TitleBar:
            glfwSetWindowPos(window, g_startWinX + dx, g_startWinY + dy);
            break;

        case HitTest::EdgeL: {
            int newW = std::max(MIN_W, g_startWinW - dx);
            int newX = g_startWinX + g_startWinW - newW;
            glfwSetWindowPos(window, newX, g_startWinY);
            glfwSetWindowSize(window, newW, g_startWinH);
            break;
        }
        case HitTest::EdgeR: {
            int newW = std::max(MIN_W, g_startWinW + dx);
            glfwSetWindowSize(window, newW, g_startWinH);
            break;
        }
        case HitTest::EdgeT: {
            int newH = std::max(MIN_H, g_startWinH - dy);
            int newY = g_startWinY + g_startWinH - newH;
            glfwSetWindowPos(window, g_startWinX, newY);
            glfwSetWindowSize(window, g_startWinW, newH);
            break;
        }
        case HitTest::EdgeB: {
            int newH = std::max(MIN_H, g_startWinH + dy);
            glfwSetWindowSize(window, g_startWinW, newH);
            break;
        }
        case HitTest::CornerTL: {
            int newW = std::max(MIN_W, g_startWinW - dx);
            int newH = std::max(MIN_H, g_startWinH - dy);
            int newX = g_startWinX + g_startWinW - newW;
            int newY = g_startWinY + g_startWinH - newH;
            glfwSetWindowPos(window, newX, newY);
            glfwSetWindowSize(window, newW, newH);
            break;
        }
        case HitTest::CornerTR: {
            int newW = std::max(MIN_W, g_startWinW + dx);
            int newH = std::max(MIN_H, g_startWinH - dy);
            int newY = g_startWinY + g_startWinH - newH;
            glfwSetWindowPos(window, g_startWinX, newY);
            glfwSetWindowSize(window, newW, newH);
            break;
        }
        case HitTest::CornerBL: {
            int newW = std::max(MIN_W, g_startWinW - dx);
            int newH = std::max(MIN_H, g_startWinH + dy);
            int newX = g_startWinX + g_startWinW - newW;
            glfwSetWindowPos(window, newX, g_startWinY);
            glfwSetWindowSize(window, newW, newH);
            break;
        }
        case HitTest::CornerBR: {
            int newW = std::max(MIN_W, g_startWinW + dx);
            int newH = std::max(MIN_H, g_startWinH + dy);
            glfwSetWindowSize(window, newW, newH);
            break;
        }
        default: break;
        }
    }

    // Cursor override для краёв
    if (g_dragActive &&
        (g_dragMode == HitTest::EdgeL || g_dragMode == HitTest::EdgeR ||
         g_dragMode == HitTest::EdgeT || g_dragMode == HitTest::EdgeB ||
         g_dragMode == HitTest::CornerTL || g_dragMode == HitTest::CornerTR ||
         g_dragMode == HitTest::CornerBL || g_dragMode == HitTest::CornerBR)) {
        SetCursorForHit(window, g_dragMode);
    } else if (!g_dragActive) {
        HitTest hover = hitTest(window, cx, cy);
        if (hover == HitTest::EdgeL || hover == HitTest::EdgeR ||
            hover == HitTest::EdgeT || hover == HitTest::EdgeB ||
            hover == HitTest::CornerTL || hover == HitTest::CornerTR ||
            hover == HitTest::CornerBL || hover == HitTest::CornerBR) {
            SetCursorForHit(window, hover);
        }
        else {
            glfwSetCursor(window, nullptr);   
        }
    }

    g_prevMouseState = state;
}

// ---------- lifecycle ----------
void Platform::init(GLFWwindow* window) {
    if (IsWayland()) {
        // На Wayland кастомный тайтлбар не работает — декорации уже системные
        return;
    }

    g_curHR = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    g_curVR = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    g_curNWSE = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    g_curNESW = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
#endif

    glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void Platform::shutdown() {
    if (g_curHR) glfwDestroyCursor(g_curHR);
    if (g_curVR) glfwDestroyCursor(g_curVR);
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    if (g_curNWSE) glfwDestroyCursor(g_curNWSE);
    if (g_curNESW) glfwDestroyCursor(g_curNESW);
#endif
    g_curHR = g_curVR = nullptr;
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    g_curNWSE = g_curNESW = nullptr;
#endif
}

// ---------- window operations ----------
void Platform::minimize(GLFWwindow* w) {
    glfwIconifyWindow(w);
}

void Platform::maximizeRestore(GLFWwindow* w) {
    if (glfwGetWindowAttrib(w, GLFW_MAXIMIZED))
        glfwRestoreWindow(w);
    else
        glfwMaximizeWindow(w);
}

void Platform::close(GLFWwindow* w) {
    glfwSetWindowShouldClose(w, GLFW_TRUE);
}

bool Platform::isMaximized(GLFWwindow* w) {
    return glfwGetWindowAttrib(w, GLFW_MAXIMIZED) != 0;
}

void Platform::applyWindowStyle(GLFWwindow*) {
    // Linux: на X11 всё делает WM, на Wayland — системные декорации.
}

// ---------- Windows-only API (заглушки, нужны для app.cc) ----------
bool Platform::maximizeHovered() { return false; }
bool Platform::maximizePressed() { return false; }

#endif // __linux__