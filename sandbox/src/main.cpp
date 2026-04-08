#include <canino/core/core.h>
#include <canino/core/memory_arena.h>
#include <canino/platform/window.h>
#include <iostream>

int main() {
    std::cout << "[Sandbox] Iniciando Engine DOD e forjando Kernel Hardware Window..." << std::endl;

    // Despacho de Config purista C-Style struct, O(1) Init
    canino::WindowDesc desc = {};
    desc.Title = "Canino Engine Executable (C++20 MSVC DOD-Strict)";
    desc.Width = 1280;
    desc.Height = 720;

    // Janela sendo cuspida puramente atrás da cortina de pointer Opaque
    canino::Window* window = canino::PlatformCreateWindow(desc);

    if (!window) {
        std::cerr << "Falha critica e abortando sub-sistema de OS." << std::endl;
        return -1;
    }

    std::cout << "[Sandbox] Instancia renderizada. Ingressando no Real-time Infinite Loop O(1).\nPressione Alt+F4 ou o X Nativo paramatar processo" << std::endl;

    // O Loop imortal que ditará os milhares de quadros por milissegundo de nossa Engine
    while (!canino::PlatformWindowShouldClose(window)) {
        
        // Ponto de respiro com Hardware nativo p/ updates
        canino::PlatformPumpMessages(window);

        // Fetch do estado
        const canino::InputState* state = canino::PlatformGetInputState(window);
        
        if (canino::IsKeyPressed(state, canino::CANINO_KEY_SPACE)) {
            std::cout << "[Input] PULO DETECTADO (Espaco)." << std::endl;
        }
        if (canino::IsKeyDown(state, canino::CANINO_KEY_W)) {
            std::cout << "[Input] W held." << std::endl;
        }
        if (canino::IsMouseButtonPressed(state, canino::CANINO_MOUSE_LEFT)) {
            std::cout << "[Input] PEW! (Click Esq)." << std::endl;
        }
        if (canino::IsKeyPressed(state, canino::CANINO_KEY_ESCAPE)) {
            std::cout << "[Input] Usuario esmagou ESCAPE, saindo gracefully." << std::endl;
            break; // Quebra o loop inves de Force Kill
        }

        // Fim do frame, limpa e atualiza os bit arrays de Previous pro proximo ciclo
        canino::PlatformUpdateInputState(window);
    }

    std::cout << "[Sandbox] Evento de Morte recebido pelo Sistema Operacional. Eliminando caches..." << std::endl;
    canino::PlatformDestroyWindow(window);

    return 0;
}
