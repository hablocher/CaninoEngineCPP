#pragma once
#include <canino/core/core.h>

namespace canino {
    // Init Setup minimalista Data-Oriented
    struct WindowDesc {
        const char* Title;
        unsigned int Width;
        unsigned int Height;
    };

    // Forward Declaration que veda tudo por trás do ponteiro; sem classe Virtual
    struct Window;

    // Interface Opaque Funcional (Sem chamadas ou vazamento pra API nativa)
    CANINO_API Window* PlatformCreateWindow(const WindowDesc& config);
    CANINO_API void PlatformDestroyWindow(Window* window);
    
    // Bombeia (polling) os O.S messages sem bloquear o fluxo. Retorna vitalidade da Janela.
    CANINO_API void PlatformPumpMessages(Window* window);
    
    // Inqueri rapidamente memória alocada, sem depender da Poll pump iterável
    CANINO_API bool PlatformWindowShouldClose(Window* window);
}
