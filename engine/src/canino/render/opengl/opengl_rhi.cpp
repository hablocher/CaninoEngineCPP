#include <canino/render/rhi.h>

#ifdef _WIN32
#include <windows.h>
// OpenGL legado estrito de C-Pipeline nativo injetavel Microsoft
#include <gl/GL.h> 

namespace canino {

// PIMPL State restrito
struct OpenGLContext {
    HDC DeviceContext;
    HGLRC RenderContext;
    HWND Window;
};

// Como jogos usam comumente Single-Window / Single-Context de placa inteira, cache unico
static OpenGLContext* s_Context = nullptr; 

bool RHI_Initialize(void* nativeWindowHandle) {
    if (s_Context) return true; // idempotente

    // Alocar buffer
    s_Context = new OpenGLContext();
    s_Context->Window = (HWND)nativeWindowHandle;
    
    // Obscura matematica pra forcar a Placa Mae ceder Contexto de Desenho
    s_Context->DeviceContext = GetDC(s_Context->Window);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    // Obriga double-buffering e flag nativa gl pra hardware acceleration
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    int pixelFormat = ChoosePixelFormat(s_Context->DeviceContext, &pfd);
    if (!pixelFormat) return false;

    if (!SetPixelFormat(s_Context->DeviceContext, pixelFormat, &pfd)) return false;

    // Subida GDI pro Hardware Bridge e setacao de StateMachine Singleton do OpenGL
    s_Context->RenderContext = wglCreateContext(s_Context->DeviceContext);
    if (!s_Context->RenderContext) return false;

    if (!wglMakeCurrent(s_Context->DeviceContext, s_Context->RenderContext)) return false;

    return true;
}

void RHI_Shutdown() {
    if (s_Context) {
        wglMakeCurrent(nullptr, nullptr);
        if (s_Context->RenderContext) wglDeleteContext(s_Context->RenderContext);
        if (s_Context->DeviceContext && s_Context->Window) ReleaseDC(s_Context->Window, s_Context->DeviceContext);
        
        delete s_Context;
        s_Context = nullptr;
    }
}

void RHI_BeginFrame() {
    // Pipeline preparatório nulo pro BasicGL
}

void RHI_EndFrame() {
    // Esmurrar a GDI com SWAP HW Pointers descarregando FrontBuffer!
    if (s_Context && s_Context->DeviceContext) {
        SwapBuffers(s_Context->DeviceContext);
    }
}

// Implementacao do Front end submissivel. Aqui é puro OpenGL nativo de Kernel C function call.
namespace RenderCommand {
    void SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void RenderCommand::DrawQuad(float x, float y, float w, float h, float r, float g, float b) {
#ifdef _WIN32
    // C-Style imemdiate Mode GL 1.1 Embutido na API Nativa pra Sandboxing sujo via Hardware
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
#endif
}

}
#endif
