namespace OurPaint {
}

#include "App.h"
#include "Platform.h"
#include "Fbo.h"
#include "OurPaintInternal.h"
#include "Events.h"
#include <array>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

ButtonHitRect g_minimizeRect, g_maximizeRect, g_closeRect;

static bool g_imguiReady = false;
static bool g_rendering = false;

bool AppIsRendering() { return g_rendering; }

bool Contains(const ButtonHitRect& r, float x, float y) {
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void UpdateCaptionHitRects(float w) {
    const float bw = 3.0f * CAPTION_BUTTON_WIDTH;
    const float sx = std::max(0.0f, w - bw - CAPTION_BUTTON_RIGHT_MARGIN);
    g_minimizeRect = {sx, CAPTION_BUTTON_TOP, CAPTION_BUTTON_WIDTH, CAPTION_BUTTON_HEIGHT};
    g_maximizeRect = {sx + CAPTION_BUTTON_WIDTH, CAPTION_BUTTON_TOP, CAPTION_BUTTON_WIDTH, CAPTION_BUTTON_HEIGHT};
    g_closeRect = {sx + 2.0f * CAPTION_BUTTON_WIDTH, CAPTION_BUTTON_TOP, CAPTION_BUTTON_WIDTH, CAPTION_BUTTON_HEIGHT};
}

static FBO::Fbo g_demoFbo;

static void RenderDemoIntoFbo(float time) {
    if (!g_demoFbo.valid()) return;

    FBO::bind(g_demoFbo);
    glViewport(0, 0, g_demoFbo.w, g_demoFbo.h);
    glEnable(GL_SCISSOR_TEST);

    constexpr int bands = 32;
    const int bandH = std::max(1, g_demoFbo.h / bands);
    for (int i = 0; i < bands; ++i) {
        const int y = i * bandH;
        const int hh = (i == bands - 1) ? (g_demoFbo.h - y) : bandH;
        if (hh <= 0) break;
        const float t = (float) i / bands;
        const float phase = t * 6.2831853f + time * 1.2f;
        const float r = 0.15f + 0.35f * (0.5f + 0.5f * std::sin(phase));
        const float g = 0.15f + 0.35f * (0.5f + 0.5f * std::sin(phase + 2.094f));
        const float b = 0.25f + 0.45f * (0.5f + 0.5f * std::sin(phase + 4.189f));
        glScissor(0, y, g_demoFbo.w, hh);
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDisable(GL_SCISSOR_TEST);
    FBO::unbind();
}

static bool DrawCaptionButton(const char *id, const ButtonHitRect& r, int kind, bool maximized) {
    ImGui::SetCursorPos(ImVec2(r.x, r.y));
    const bool clicked = ImGui::InvisibleButton(id, ImVec2(r.w, r.h));

    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (kind == 1) {
        hovered = hovered || Platform::maximizeHovered();
        active = active || Platform::maximizePressed();
    }

    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    ImDrawList *draw = ImGui::GetWindowDrawList();

    if (kind == 2 && hovered)
        draw->AddRectFilled(p0, p1, IM_COL32(196, 43, 28, active ? 255 : 235));
    else if (hovered)
        draw->AddRectFilled(p0, p1, IM_COL32(75, 79, 87, active ? 220 : 165));

    const ImU32 iconColor = IM_COL32(238, 240, 243, 255);
    const float cx = (p0.x + p1.x) * 0.5f;
    const float cy = (p0.y + p1.y) * 0.5f;

    if (kind == 0) {
        draw->AddLine(ImVec2(cx - 6.0f, cy + 4.0f), ImVec2(cx + 6.0f, cy + 4.0f), iconColor, 1.4f);
    } else if (kind == 1) {
        if (!maximized) {
            draw->AddRect(ImVec2(cx - 5.5f, cy - 5.0f), ImVec2(cx + 5.5f, cy + 5.5f), iconColor, 0.0f, 0, 1.2f);
        } else {
            draw->AddRect(ImVec2(cx - 3.0f, cy - 5.5f), ImVec2(cx + 6.0f, cy + 3.5f), iconColor, 0.0f, 0, 1.2f);
            draw->AddRect(ImVec2(cx - 6.0f, cy - 2.0f), ImVec2(cx + 3.0f, cy + 6.0f), iconColor, 0.0f, 0, 1.2f);
        }
    } else {
        draw->AddLine(ImVec2(cx - 5.0f, cy - 5.0f), ImVec2(cx + 5.0f, cy + 5.0f), iconColor, 1.35f);
        draw->AddLine(ImVec2(cx + 5.0f, cy - 5.0f), ImVec2(cx - 5.0f, cy + 5.0f), iconColor, 1.35f);
    }

    return clicked;
}

namespace {
    std::array<bool, GLFW_KEY_LAST + 1> g_keysDown{};

    void OnKeyEvent(GLFWwindow *, int key, int, int action, int mods) {
        if (key < 0 || key > GLFW_KEY_LAST) return;
        if (action == GLFW_PRESS || action == GLFW_REPEAT) g_keysDown[static_cast<size_t>(key)] = true;
        else if (action == GLFW_RELEASE) g_keysDown[static_cast<size_t>(key)] = false;
        else return;

        OurPaint::Event event;
        event.type = action == GLFW_RELEASE ? OurPaint::EventType::KeyReleased : OurPaint::EventType::KeyPressed;
        event.key = key;
        event.mods = mods;
        event.repeat = action == GLFW_REPEAT;
        for (int i = 0; i <= GLFW_KEY_LAST; ++i)
            if (g_keysDown[static_cast<size_t>(i)]) event.keysDown.push_back(i);
        OurPaint::EmitEvent(event);
    }
}

void RenderFrame(GLFWwindow *window) {
    if (!g_imguiReady || g_rendering) return;

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return;

    g_rendering = true;

    constexpr float TOOLBAR_HEIGHT = 330.0f;
    constexpr float TOOL_BUTTON_WIDTH = 220.0f;
    constexpr float TOOL_BUTTON_HEIGHT = 28.0f;
    constexpr float TOOL_BUTTON_GAP = 6.0f;

    const float frameW = std::min(720.0f, (float) w - 80.0f);
    const float frameH = std::min(420.0f, (float) h - TITLEBAR_HEIGHT - TOOLBAR_HEIGHT - 80.0f);
    const ImVec2 framePos((w - frameW) * 0.5f,
                          TITLEBAR_HEIGHT + TOOLBAR_HEIGHT + ((h - TITLEBAR_HEIGHT - TOOLBAR_HEIGHT) - frameH) * 0.5f);
    const ImVec2 framePosMax(framePos.x + frameW, framePos.y + frameH);

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    const float dpi = (w > 0) ? (float) fbW / (float) w : 1.0f;
    const int targetW = (int) std::max(1.0f, frameW * dpi);
    const int targetH = (int) std::max(1.0f, frameH * dpi);

    if (!g_demoFbo.valid() || g_demoFbo.w != targetW || g_demoFbo.h != targetH)
        FBO::resize(g_demoFbo, targetW, targetH);

    if (g_demoFbo.valid()) {
        if (OurPaint::Internal::renderer()) {
            FBO::bind(g_demoFbo);
            glViewport(0, 0, g_demoFbo.w, g_demoFbo.h);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.075f, 0.09f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            OurPaint::Internal::renderer()(g_demoFbo.w, g_demoFbo.h, glfwGetTime());
            glDisable(GL_DEPTH_TEST);
            FBO::unbind();
        } else {
            RenderDemoIntoFbo((float) glfwGetTime());
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    Platform::updateDrag(window);

    ImGui::NewFrame();
    UpdateCaptionHitRects((float) w);

    const float windowWidthF = (float) w;
    const bool maximized = Platform::isMaximized(window);

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidthF, TITLEBAR_HEIGHT), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.84f, 0.85f, 0.87f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.30f, 0.45f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.42f, 0.62f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.24f, 0.37f, 1.0f));

    ImGui::Begin("##CustomTitleBar", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoNavFocus);

    ImDrawList *titleDraw = ImGui::GetWindowDrawList();
    const ImVec2 titlePos = ImGui::GetWindowPos();
    titleDraw->AddRectFilled(titlePos,
                             ImVec2(titlePos.x + windowWidthF, titlePos.y + TITLEBAR_HEIGHT),
                             IM_COL32(205, 208, 213, 255));

    ImGui::SetCursorPos(ImVec2(14.0f, 16.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(65, 68, 74, 255));
    ImGui::TextUnformatted("OurPaint");
    ImGui::PopStyleColor();
    {
        const ImVec2 textMin = ImGui::GetItemRectMin();
        const ImVec2 textSize = ImGui::GetItemRectSize();
        const float lineY = textMin.y + textSize.y + 3.0f;
        titleDraw->AddLine(ImVec2(textMin.x, lineY),
                           ImVec2(textMin.x + textSize.x, lineY),
                           IM_COL32(150, 154, 160, 255), 1.0f);
    }

    const bool minimizeClicked = DrawCaptionButton("##CaptionMinimize", g_minimizeRect, 0, maximized);
    const bool maximizeClicked = DrawCaptionButton("##CaptionMaximize", g_maximizeRect, 1, maximized);
    const bool closeClicked = DrawCaptionButton("##CaptionClose", g_closeRect, 2, maximized);

    ImGui::SetCursorPos(ImVec2(104.0f, 9.0f));
    ImGui::Button("Project", ImVec2(88.0f, 30.0f));

    ImGui::End();
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(3);

    ImGui::SetNextWindowPos(ImVec2(0.0f, TITLEBAR_HEIGHT), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidthF, TOOLBAR_HEIGHT), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.91f, 0.92f, 0.93f, 1.0f));
    ImGui::Begin("##ToolsToolbar", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                 ImGuiWindowFlags_NoNavFocus);

    if (ImGui::Button("Point", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Point);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Line", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Line);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Cubic Bezier", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CubicBezier);

    ImGui::NewLine();

    if (ImGui::Button("Circle by radius", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleByRadius);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Circle by diameter", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleByDiameter);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Circle by two points", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleByTwoPoints);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Circle by three points", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleByThreePoints);

    ImGui::NewLine();

    if (ImGui::Button("Circle tangent two lines", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleTangentToTwoLines);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Circle tangent three lines", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::CircleTangentToThreeLines);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Arc by radius", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::ArcByRadius);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Arc by diameter", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::ArcByDiameter);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Arc by three points", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::ArcByThreePoints);

    ImGui::NewLine();

    if (ImGui::Button("Distance", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Distance);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("H/V Distance", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::HVDistance);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Angle", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Angle);

    ImGui::NewLine();

    if (ImGui::Button("Coincidence", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Coincidence);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Verticality", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Verticality);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Horizontality", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Horizontality);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Parallelism", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Parallelism);

    ImGui::NewLine();

    if (ImGui::Button("Perpendicularity", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Perpendicularity);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Equality", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Equality);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Tangent", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Tangent);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Fix / Unfix", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::FixUnfix);

    ImGui::NewLine();

    if (ImGui::Button("Concentricity", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Concentricity);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Collinearity", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Collinearity);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Symmetry", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Symmetry);
    ImGui::SameLine(0.0f, TOOL_BUTTON_GAP);

    if (ImGui::Button("Midpoint", ImVec2(TOOL_BUTTON_WIDTH, TOOL_BUTTON_HEIGHT)))
        OurPaint::Internal::notifyToolButtonClicked(OurPaint::Tool::Midpoint);

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);

    {
        ImDrawList *fg = ImGui::GetForegroundDrawList();
        if (g_demoFbo.valid()) {
            fg->AddImage((ImTextureID) (intptr_t) g_demoFbo.tex,
                         framePos, framePosMax,
                         ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
        } else {
            fg->AddRectFilled(framePos, framePosMax, IM_COL32(75, 30, 35, 255));
            const char *msg = "OpenGL not embedded";
            const ImVec2 ts = ImGui::CalcTextSize(msg);
            fg->AddText(ImVec2(framePos.x + (frameW - ts.x) * 0.5f,
                               framePos.y + (frameH - ts.y) * 0.5f),
                        IM_COL32(255, 200, 200, 255), msg);
        }
        fg->AddRect(framePos, framePosMax, IM_COL32(85, 95, 115, 255), 0.0f, 0, 2.0f);
    }

    if (minimizeClicked) Platform::minimize(window);
    if (maximizeClicked) Platform::maximizeRestore(window);
    if (closeClicked) Platform::close(window);

    ImGui::Render();

    glViewport(0, 0, fbW, fbH);
    glClearColor(0.88f, 0.89f, 0.91f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    g_rendering = false;
}

void AppInit(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.88f, 0.89f, 0.91f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.68f, 0.69f, 0.71f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.80f, 0.81f, 0.83f, 1.0f);
    glfwSetKeyCallback(window, OnKeyEvent);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    FBO::init();
    g_imguiReady = true;
}

void AppShutdown() {
    g_imguiReady = false;
    FBO::destroy(g_demoFbo);
    FBO::shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}