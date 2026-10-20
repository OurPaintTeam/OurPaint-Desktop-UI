#include <OurPaint.h>
#include "App.h"
#include "Platform.h"
#include <GLFW/glfw3.h>
#include <cstdio>

#include "Events.h"

namespace {
OurPaint::ViewportRenderer g_renderer;

std::string g_title = "OurPaint";
}
namespace OurPaint {
void setViewportRenderer(ViewportRenderer renderer) { g_renderer = std::move(renderer); }

}
namespace OurPaint::Internal {
const ViewportRenderer& renderer() { return g_renderer; }
void notifyToolButtonClicked(Tool tool) { EmitEvent(Event{EventType::ButtonClicked, tool}); }
void setTitle(const char* title) { if (title && *title) g_title = title; }
}

int OurPaint::run(const Config& config) {
#ifdef FORCE_X11_ON_LINUX
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
    if (!glfwInit()) { std::fprintf(stderr, "OurPaint: GLFW initialization failed\\n"); return 1; }
#if defined(__linux__)
    const bool wayland = glfwGetPlatform() == GLFW_PLATFORM_WAYLAND;
    glfwWindowHint(GLFW_DECORATED, wayland ? GLFW_TRUE : GLFW_FALSE);
#else
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    if (!window) { std::fprintf(stderr, "OurPaint: window creation failed\\n"); glfwTerminate(); return 2; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.vsync ? 1 : 0);
    Platform::init(window);
    AppInit(window);

    // Prepare several complete frames while the window is still hidden. This
    // initializes the embedded FBO/texture and lets the platform layer settle
    // before the first visible frame.
    RenderFrame(window);
    glfwPollEvents();

    int w = 0, h = 0;
    glfwGetWindowSize(window, &w, &h);
    if (w > 0 && h > 0)
        glfwSetWindowSize(window, w, h);
    glfwPollEvents();

    for (int i = 0; i < 3; ++i) {
        glfwPollEvents();
        RenderFrame(window);
    }

    glfwShowWindow(window);
    glfwPollEvents();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        RenderFrame(window);
    }
    AppShutdown();
    Platform::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
