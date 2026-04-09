#include <canino/core/memory_arena.h>
#include <canino/platform/window.h>
#include <canino/render/rhi.h>
#include <canino/math/math_ops.h>
#include <iostream>

int main() {
    std::cout << "[Demo2] Engine 3D Ativada. Renderizando Cubo Fotorealista." << std::endl;

    canino::MemoryArena arena; arena.InitializeHeap(1024 * 1024 * 8);
    canino::WindowDesc desc = { "Canino 3D Simulator", 800, 600 };
    canino::Window* window = canino::PlatformCreateWindow(desc);
    
    canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window));

    // Carrega o JPG via Gateway RHI e assume tamanho fixo geometrico
    void* caninoTex = canino::RenderCommand::CreateTexture("assets/canino.jpg");
    if (!caninoTex) caninoTex = canino::RenderCommand::CreateTexture("E:/Code/CPP/CaninoEngine/assets/canino.jpg");
    
    if (!caninoTex) std::cout << "Falha massiva ao tentar engolir JPG. Trabalhando em cega." << std::endl;

    float rotX = 0.0f;
    float rotY = 0.0f;
    bool autoSpin = false;
    bool spacePressedState = false;

    while (!canino::PlatformWindowShouldClose(window)) {
        canino::PlatformPumpMessages(window);

        const canino::InputState* state = canino::PlatformGetInputState(window);
        if (canino::IsKeyPressed(state, canino::CANINO_KEY_ESCAPE)) break;

        // Space Toggle Auto-Spin
        bool currentSpace = canino::IsKeyDown(state, canino::CANINO_KEY_SPACE);
        if (currentSpace && !spacePressedState) {
            autoSpin = !autoSpin;
            std::cout << "Auto-Spin: " << (autoSpin ? "LIGADO" : "DESLIGADO") << std::endl;
        }
        spacePressedState = currentSpace;

        // Manual Spin
        if (canino::IsKeyDown(state, canino::CANINO_KEY_W)) rotX += 0.05f;
        if (canino::IsKeyDown(state, canino::CANINO_KEY_S)) rotX -= 0.05f;
        if (canino::IsKeyDown(state, canino::CANINO_KEY_A)) rotY += 0.05f;
        if (canino::IsKeyDown(state, canino::CANINO_KEY_D)) rotY -= 0.05f;

        if (autoSpin) {
            rotX += 0.01f;
            rotY += 0.02f;
        }

        // ====== Pipeline Math MVP (Model-View-Projection) ======
        canino::Mat4 scaleMatrix = canino::math::Mat4Scale(2.0f, 2.0f, 2.0f);
        canino::Mat4 modelX = canino::math::Mat4RotateX(rotX);
        canino::Mat4 modelY = canino::math::Mat4RotateY(rotY);
        
        canino::Mat4 modelRot = canino::math::Mat4Multiply(modelX, modelY);
        canino::Mat4 model = canino::math::Mat4Multiply(scaleMatrix, modelRot);
        
        // Cubo de diametro 4 (raio 2.0) posicionado a 8 de profundidade
        canino::Mat4 view = canino::math::Mat4Translate(0.0f, 0.0f, 8.0f);
        
        canino::Mat4 proj = canino::math::Mat4Perspective(3.1415f / 3.0f, 800.0f / 600.0f, 0.1f, 100.0f);

        canino::Mat4 mv = canino::math::Mat4Multiply(model, view);
        canino::Mat4 mvp = canino::math::Mat4Multiply(mv, proj);

        // Graphic Frontend
        canino::RHI_BeginFrame();
        
        canino::RenderCommand::SetClearColor(0.2f, 0.2f, 0.25f, 1.0f);
        canino::RenderCommand::Clear(); // Irá esvaziar Z-Buffer e ColorBuffer Nativos
        
        // Render 3D Implacavel (Sem lixo de classes GameObject)
        canino::RenderCommand::DrawCube(mvp, caninoTex);

        canino::RHI_EndFrame();
    }

    canino::RHI_Shutdown();
    canino::PlatformDestroyWindow(window);
    return 0;
}
