#ifndef OURPAINT_OURPAINT_H
#define OURPAINT_OURPAINT_H

#include <functional>
#include <string>

namespace OurPaint {

// Called while the library's viewport FBO is bound and the OpenGL context is current.
// Render your scene into the supplied pixel dimensions. Do not swap buffers.
using ViewportRenderer = std::function<void(int width, int height, double timeSeconds)>;

struct Config {
    std::string title = "OurPaint";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

// Optional: if no renderer is set, the library draws its built-in animated fallback.
void setViewportRenderer(ViewportRenderer renderer);

int run(const Config& config = Config{});

} // namespace OurPaint

#endif // OURPAINT_OURPAINT_H
