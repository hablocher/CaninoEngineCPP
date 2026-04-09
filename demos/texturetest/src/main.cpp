#include <canino/platform/window.h>
#include <canino/platform/input.h>
#include <canino/render/rhi.h>
#include <canino/math/math_ops.h>
#include <iostream>

int main() {
    std::cout << "Inicializando Texture Test Arena..." << std::endl;
    // O sistema de janela padrão 800x600 da engine
    canino::WindowDesc desc = { "Canino Texture Test", 800, 600 };
    canino::Window* window = canino::PlatformCreateWindow(desc);
    canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window));

    // Forçaremos o Load de uma textura arbitrária
    void* bgTexture = canino::RenderCommand::CreateTexture("assets/floor_texture.jpg");
    
    if (bgTexture) std::cout << "[TEST] Textura Floor alocada na placa com sucesso!" << std::endl;
    else std::cout << "[TEST] FALHOU AO LER TEXTURA DO DISCO O.S." << std::endl;

    // Matrizes estáticas
    canino::Mat4 identity = canino::math::Mat4Identity();
    // Move o plano levemente pra frente pra Z-Buffer renderizar
    canino::Mat4 model = canino::math::Mat4Translate(0.0f, 0.0f, 1.5f);
    // Perspectiva Clássica
    canino::Mat4 proj = canino::math::Mat4Perspective(1.0f, 800.0f/600.0f, 0.1f, 10.0f);
    
    canino::Mat4 mvp = canino::math::Mat4Multiply(model, proj);

    while (!canino::PlatformWindowShouldClose(window)) {
        canino::PlatformPumpMessages(window);
        canino::RHI_BeginFrame();
        
        canino::RenderCommand::SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        canino::RenderCommand::Clear();

        // Isso vai renderizar um cubo gigante texturizado no meio da tela
        canino::RenderCommand::DrawCube(mvp, bgTexture);

        canino::RHI_EndFrame();
    }

    canino::RHI_Shutdown();
    return 0;
}
