#include <canino/core/core.h>
#include <canino/core/memory_arena.h>
#include <canino/platform/window.h>
#include <canino/render/rhi.h>
#include <canino/ecs/registry.h>
#include <canino/math/math_ops.h>
#include <iostream>

struct CameraTarget {
    float r, g, b;
};

int main() {
    std::cout << "[Sandbox] Iniciando Engine DOD e forjando Kernel Hardware Window..." << std::endl;

    // Despacho de Config purista C-Style struct, O(1) Init
    canino::WindowDesc desc = {};
    desc.Title = "Canino Engine RHI Subsystem";
    desc.Width = 1280;
    desc.Height = 720;

    canino::Window* window = canino::PlatformCreateWindow(desc);
    if (!window) return -1;

    // PLUG INFALLÍVEL DOD ENTRE JANELA HW E DEVICE GRáFICO GL
    if (!canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window))) {
        std::cerr << "Falha Massiva em Subir OpenGL Context! GPU Rejeitada." << std::endl;
        return -2;
    }

    std::cout << "[Sandbox] Render Hardware Interface Ativa. Real-time Infinity Loop inicializado." << std::endl;

    // Criando a Galaxia Principal de Gerenciamento Estrito DOD
    canino::Registry registry;
    
    canino::Entity player = registry.CreateEntity();
    registry.AddComponent<CameraTarget>(player, { 0.1f, 0.2f, 0.4f });

    // O Loop imortal que ditará os milhares de quadros por milissegundo de nossa Engine
    while (!canino::PlatformWindowShouldClose(window)) {
        
        // Poller do SO
        canino::PlatformPumpMessages(window);

        // Submissoes passivas puramente baseadas em estado pro Graphic Hardware
        const canino::InputState* state = canino::PlatformGetInputState(window);
        
        // Em vez de varíaveis hardcoded, os Componentes puros sofrem Mutação através dos Sistemas de update
        CameraTarget* cam = registry.GetComponent<CameraTarget>(player);

        // Easter egg de Input DOD controlando entidade camera dinamicamente
        if (cam) {
            if (canino::IsKeyDown(state, canino::CANINO_KEY_SPACE)) { cam->g += 0.01f; }
            if (canino::IsKeyDown(state, canino::CANINO_KEY_D)) { cam->r += 0.01f; }
            if (canino::IsKeyDown(state, canino::CANINO_KEY_A)) { cam->r -= 0.01f; }
            
            if (cam->r > 1.0f) cam->r = 0.0f; else if (cam->r < 0.0f) cam->r = 1.0f;
            if (cam->g > 1.0f) cam->g = 0.0f; else if (cam->g < 0.0f) cam->g = 1.0f;
        }

        if (canino::IsKeyPressed(state, canino::CANINO_KEY_ESCAPE)) break;

        // ---------- RHI FRONT-END SIMULATION -----------
        canino::RHI_BeginFrame();
        
        if (cam) canino::RenderCommand::SetClearColor(cam->r, cam->g, cam->b, 1.0f);
        
        // Descarrega Comando submetido na Placa de Vídeo
        canino::RenderCommand::Clear();
        
        // Inverte FrontBuffer com Backbuffer de SWAP_CHAIN O.S
        canino::RHI_EndFrame();
        // -----------------------------------------------

        // Fim do frame, limpa buffers
        canino::PlatformUpdateInputState(window);
    }

    canino::RHI_Shutdown();
    canino::PlatformDestroyWindow(window);

    return 0;
}
