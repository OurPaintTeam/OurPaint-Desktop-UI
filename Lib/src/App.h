#ifndef OURPAINT_APP_H
#define OURPAINT_APP_H

#include <GLFW/glfw3.h>

// --- Layout constants shared between app.cc and platform_*.cc ---
inline constexpr float TITLEBAR_HEIGHT              = 48.0f;
inline constexpr float CAPTION_BUTTON_WIDTH         = 46.0f;
inline constexpr float CAPTION_BUTTON_HEIGHT        = TITLEBAR_HEIGHT;
inline constexpr float CAPTION_BUTTON_TOP           = 0.0f;
inline constexpr float CAPTION_BUTTON_RIGHT_MARGIN  = 0.0f;
inline constexpr float TOOL_BUTTON_X                = 12.0f;
inline constexpr float TOOL_BUTTON_Y                = 8.0f;
inline constexpr float TOOL_BUTTON_WIDTH            = 82.0f;
inline constexpr float TOOL_BUTTON_HEIGHT           = 28.0f;
inline constexpr float TOOL_BUTTON_GAP              = 8.0f;

struct ButtonHitRect { float x, y, w, h; };

extern ButtonHitRect g_minimizeRect;
extern ButtonHitRect g_maximizeRect;
extern ButtonHitRect g_closeRect;

bool Contains(const ButtonHitRect& r, float x, float y);
void UpdateCaptionHitRects(float clientWidth);

void AppInit(GLFWwindow* window);
void AppShutdown();
void RenderFrame(GLFWwindow* window);
bool AppIsRendering();

#endif // OURPAINT_APP_H
