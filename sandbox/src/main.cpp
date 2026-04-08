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

        // [TODO RENDER LOOP e SIMULATION]

    }

    std::cout << "[Sandbox] Evento de Morte recebido pelo Sistema Operacional. Eliminando caches..." << std::endl;
    canino::PlatformDestroyWindow(window);

    return 0;
}
