namespace OurPaint { } // OurPaint library namespace

#include "Fbo.h"
#include <algorithm>
#include <cstdio>

// ---------------------------------------------------------------------------
//  GL constants (отсутствуют в <GL/gl.h> от MinGW / в gl.h ниже GL 3.0)
// ---------------------------------------------------------------------------
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif
#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif
#ifndef GL_TEXTURE_2D
#define GL_TEXTURE_2D 0x0DE1
#endif
#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER 0x8D41
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

// ---------- GL function pointers ----------
typedef void   (APIENTRY *PFN_GLGENFRAMEBUFFERS)(GLsizei, GLuint*);
typedef void   (APIENTRY *PFN_GLBINDFRAMEBUFFER)(GLenum, GLuint);
typedef void   (APIENTRY *PFN_GLFRAMEBUFFERTEXTURE2D)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef GLenum (APIENTRY *PFN_GLCHECKFRAMEBUFFERSTATUS)(GLenum);
typedef void   (APIENTRY *PFN_GLDELETEFRAMEBUFFERS)(GLsizei, const GLuint*);
typedef void   (APIENTRY *PFN_GLGENRENDERBUFFERS)(GLsizei, GLuint*);
typedef void   (APIENTRY *PFN_GLBINDRENDERBUFFER)(GLenum, GLuint);
typedef void   (APIENTRY *PFN_GLRENDERBUFFERSTORAGE)(GLenum, GLenum, GLsizei, GLsizei);
typedef void   (APIENTRY *PFN_GLFRAMEBUFFERRENDERBUFFER)(GLenum, GLenum, GLenum, GLuint);
typedef void   (APIENTRY *PFN_GLDELETERENDERBUFFERS)(GLsizei, const GLuint*);

static PFN_GLGENFRAMEBUFFERS         p_glGenFramebuffers         = nullptr;
static PFN_GLBINDFRAMEBUFFER         p_glBindFramebuffer         = nullptr;
static PFN_GLFRAMEBUFFERTEXTURE2D    p_glFramebufferTexture2D    = nullptr;
static PFN_GLCHECKFRAMEBUFFERSTATUS  p_glCheckFramebufferStatus  = nullptr;
static PFN_GLDELETEFRAMEBUFFERS      p_glDeleteFramebuffers      = nullptr;
static PFN_GLGENRENDERBUFFERS        p_glGenRenderbuffers        = nullptr;
static PFN_GLBINDRENDERBUFFER        p_glBindRenderbuffer        = nullptr;
static PFN_GLRENDERBUFFERSTORAGE     p_glRenderbufferStorage     = nullptr;
static PFN_GLFRAMEBUFFERRENDERBUFFER p_glFramebufferRenderbuffer = nullptr;
static PFN_GLDELETERENDERBUFFERS     p_glDeleteRenderbuffers     = nullptr;

static bool g_glLoaded = false;

namespace FBO {

bool init() {
    if (g_glLoaded) return true;

    p_glGenFramebuffers         = (PFN_GLGENFRAMEBUFFERS)glfwGetProcAddress("glGenFramebuffers");
    p_glBindFramebuffer         = (PFN_GLBINDFRAMEBUFFER)glfwGetProcAddress("glBindFramebuffer");
    p_glFramebufferTexture2D    = (PFN_GLFRAMEBUFFERTEXTURE2D)glfwGetProcAddress("glFramebufferTexture2D");
    p_glCheckFramebufferStatus  = (PFN_GLCHECKFRAMEBUFFERSTATUS)glfwGetProcAddress("glCheckFramebufferStatus");
    p_glDeleteFramebuffers      = (PFN_GLDELETEFRAMEBUFFERS)glfwGetProcAddress("glDeleteFramebuffers");
    p_glGenRenderbuffers        = (PFN_GLGENRENDERBUFFERS)glfwGetProcAddress("glGenRenderbuffers");
    p_glBindRenderbuffer        = (PFN_GLBINDRENDERBUFFER)glfwGetProcAddress("glBindRenderbuffer");
    p_glRenderbufferStorage     = (PFN_GLRENDERBUFFERSTORAGE)glfwGetProcAddress("glRenderbufferStorage");
    p_glFramebufferRenderbuffer = (PFN_GLFRAMEBUFFERRENDERBUFFER)glfwGetProcAddress("glFramebufferRenderbuffer");
    p_glDeleteRenderbuffers     = (PFN_GLDELETERENDERBUFFERS)glfwGetProcAddress("glDeleteRenderbuffers");

    g_glLoaded = p_glGenFramebuffers && p_glBindFramebuffer && p_glFramebufferTexture2D &&
                 p_glCheckFramebufferStatus && p_glDeleteFramebuffers &&
                 p_glGenRenderbuffers && p_glBindRenderbuffer &&
                 p_glRenderbufferStorage && p_glFramebufferRenderbuffer &&
                 p_glDeleteRenderbuffers;
    return g_glLoaded;
}

void shutdown() {
    // Per-instance FBOs должны быть уничтожены через destroy().
    // Здесь только сбрасываем загруженные указатели (не обязательно).
    g_glLoaded = false;
}

void destroy(Fbo& fbo) {
    if (fbo.id != 0 && p_glDeleteFramebuffers)
        p_glDeleteFramebuffers(1, &fbo.id);
    fbo.id = 0;

    if (fbo.tex != 0) {
        glDeleteTextures(1, &fbo.tex);
        fbo.tex = 0;
    }

    if (fbo.depth != 0 && p_glDeleteRenderbuffers)
        p_glDeleteRenderbuffers(1, &fbo.depth);
    fbo.depth = 0;

    fbo.w = 0;
    fbo.h = 0;
}

bool resize(Fbo& fbo, int w, int h) {
    if (w <= 0 || h <= 0) return false;
    if (fbo.valid() && fbo.w == w && fbo.h == h) return true;

    if (!p_glGenFramebuffers) return false;

    destroy(fbo);

    p_glGenFramebuffers(1, &fbo.id);
    p_glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);

    glGenTextures(1, &fbo.tex);
    glBindTexture(GL_TEXTURE_2D, fbo.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    p_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.tex, 0);

    p_glGenRenderbuffers(1, &fbo.depth);
    p_glBindRenderbuffer(GL_RENDERBUFFER, fbo.depth);
    p_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    p_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo.depth);

    const bool ok = (p_glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    p_glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!ok) {
        destroy(fbo);
        return false;
    }

    fbo.w = w;
    fbo.h = h;
    return true;
}

void bind(const Fbo& fbo) {
    if (fbo.valid() && p_glBindFramebuffer)
        p_glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
}

void unbind() {
    if (p_glBindFramebuffer)
        p_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace FBO