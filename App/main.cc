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
        [](const OurPaint::Event& event) {
            using namespace OurPaint;

            switch (event.type) {
                case EventType::ButtonClicked:
                    switch (event.tool) {
                        case Tool::None:
                            std::cout << "[App] None selected\n";
                            break;

                        case Tool::Point:
                            std::cout << "[App] Point selected\n";
                            break;
                        case Tool::Line:
                            std::cout << "[App] Line selected\n";
                            break;
                        case Tool::CubicBezier:
                            std::cout << "[App] Cubic Bezier selected\n";
                            break;

                        case Tool::CircleByRadius:
                            std::cout << "[App] Circle by radius selected\n";
                            break;
                        case Tool::CircleByDiameter:
                            std::cout << "[App] Circle by diameter selected\n";
                            break;
                        case Tool::CircleByTwoPoints:
                            std::cout << "[App] Circle by two points selected\n";
                            break;
                        case Tool::CircleByThreePoints:
                            std::cout << "[App] Circle by three points selected\n";
                            break;
                        case Tool::CircleTangentToTwoLines:
                            std::cout << "[App] Circle tangent to two lines selected\n";
                            break;
                        case Tool::CircleTangentToThreeLines:
                            std::cout << "[App] Circle tangent to three lines selected\n";
                            break;

                        case Tool::ArcByRadius:
                            std::cout << "[App] Arc by radius selected\n";
                            break;
                        case Tool::ArcByDiameter:
                            std::cout << "[App] Arc by diameter selected\n";
                            break;
                        case Tool::ArcByThreePoints:
                            std::cout << "[App] Arc by three points selected\n";
                            break;

                        case Tool::Distance:
                            std::cout << "[App] Distance selected\n";
                            break;
                        case Tool::HVDistance:
                            std::cout << "[App] H/V Distance selected\n";
                            break;
                        case Tool::Angle:
                            std::cout << "[App] Angle selected\n";
                            break;

                        case Tool::Coincidence:
                            std::cout << "[App] Coincidence selected\n";
                            break;
                        case Tool::Verticality:
                            std::cout << "[App] Verticality selected\n";
                            break;
                        case Tool::Horizontality:
                            std::cout << "[App] Horizontality selected\n";
                            break;
                        case Tool::Parallelism:
                            std::cout << "[App] Parallelism selected\n";
                            break;
                        case Tool::Perpendicularity:
                            std::cout << "[App] Perpendicularity selected\n";
                            break;
                        case Tool::Equality:
                            std::cout << "[App] Equality selected\n";
                            break;
                        case Tool::Tangent:
                            std::cout << "[App] Tangent selected\n";
                            break;
                        case Tool::FixUnfix:
                            std::cout << "[App] Fix/Unfix selected\n";
                            break;
                        case Tool::Concentricity:
                            std::cout << "[App] Concentricity selected\n";
                            break;
                        case Tool::Collinearity:
                            std::cout << "[App] Collinearity selected\n";
                            break;
                        case Tool::Symmetry:
                            std::cout << "[App] Symmetry selected\n";
                            break;
                        case Tool::Midpoint:
                            std::cout << "[App] Midpoint selected\n";
                            break;

                        default:
                            std::cout << "[App] Unknown tool selected\n";
                            break;
                    }
                    break;
                case EventType::KeyPressed:
                case EventType::KeyReleased:
                    std::cout << (event.type == EventType::KeyPressed ? "[App] Key pressed: " : "[App] Key released: ")
                            << event.key << " | held keys:";
                    for (const int key: event.keysDown) std::cout << ' ' << key;
                    std::cout << '\n';
                    break;
            }
        });

    // App owns this callback. It runs inside Lib's viewport FBO with a current GL context.
    // OurPaint::setViewportRenderer([](int width, int height, double time) {
    //     glViewport(0, 0, width, height);
    //     const float pulse = 0.5f + 0.5f * static_cast<float>(std::sin(time));
    //     glClearColor(0.06f + 0.10f * pulse, 0.12f, 0.20f + 0.12f * (1.0f - pulse), 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     // Replace this body with your own OpenGL scene renderer.
    // });
    return OurPaint::run(config);
}
