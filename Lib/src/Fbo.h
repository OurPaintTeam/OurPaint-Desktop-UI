#ifndef OURPAINT_FBO_H
#define OURPAINT_FBO_H

// fbo.h
#pragma once
#include <GLFW/glfw3.h>

namespace FBO {

    struct Fbo {
        GLuint id = 0;
        GLuint tex = 0;
        GLuint depth = 0;
        int w = 0, h = 0;
        bool valid() const { return id != 0; }
    };

    bool init();                          // один раз — грузит GL-указатели
    void shutdown();

    bool resize(Fbo& fbo, int w, int h);  // (re)create при смене размера
    void destroy(Fbo& fbo);
    void bind(const Fbo& fbo);
    void unbind();

} // namespace FBO

#endif // OURPAINT_FBO_H
