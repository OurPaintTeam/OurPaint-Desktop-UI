#ifndef OURPAINT_PLATFORM_H
#define OURPAINT_PLATFORM_H

#include <GLFW/glfw3.h>

enum class HitTest {
    Client, TitleBar, MinBtn, MaxBtn, CloseBtn, AppBtn,
    EdgeL, EdgeR, EdgeT, EdgeB,
    CornerTL, CornerTR, CornerBL, CornerBR
};

namespace Platform {
    void init(GLFWwindow* window);
    void shutdown();

    HitTest hitTest(GLFWwindow* window, double x, double y);
    void updateDrag(GLFWwindow* window);          // только Linux (X11)

    void minimize(GLFWwindow* window);
    void maximizeRestore(GLFWwindow* window);
    void close(GLFWwindow* window);
    bool isMaximized(GLFWwindow* window);
    void applyWindowStyle(GLFWwindow* window);    // DWM, Wayland fallback и т.п.
    bool maximizeHovered();
    bool maximizePressed();
}

#endif // OURPAINT_PLATFORM_H
