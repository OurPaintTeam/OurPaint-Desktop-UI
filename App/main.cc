#include <OurPaint.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include <Events.h>

int main() {
    OurPaint::Config config;
    config.title = "OurPaint - App example";
    config.width = 1280;
    config.height = 720;

    OurPaint::SetEventCallback(
            [](const OurPaint::Event& event)
            {
                using namespace OurPaint;

                switch (event.type)
                {
                    case EventType::ButtonClicked :
                        switch (event.tool)
                        {
                            case Tool::Point:
                                std::cout << "[App] Point selected\n";
                                break;

                            case Tool::Line:
                                std::cout << "[App] Line selected\n";
                                break;

                            case Tool::Circle:
                                std::cout << "[App] Circle selected\n";
                                break;
                            case Tool::Arc:
                                std::cout << "[App] Arc selected\n";
                                break;

                            default:
                                break;
                        }
                        break;

                    case EventType::KeyPressed:
                    case EventType::KeyReleased:
                        std::cout << (event.type == EventType::KeyPressed ? "[App] Key pressed: " : "[App] Key released: ")
                                  << event.key << " | held keys:";
                        for (const int key : event.keysDown) std::cout << ' ' << key;
                        std::cout << '\n';
                        break;
                }
            });

    // App owns this callback. It runs inside Lib's viewport FBO with a current GL context.
    OurPaint::setViewportRenderer([](int width, int height, double time) {
        glViewport(0, 0, width, height);
        const float pulse = 0.5f + 0.5f * static_cast<float>(std::sin(time));
        glClearColor(0.06f + 0.10f * pulse, 0.12f, 0.20f + 0.12f * (1.0f - pulse), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Replace this body with your own OpenGL scene renderer.
    });
    return OurPaint::run(config);
}
