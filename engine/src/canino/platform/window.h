#pragma once
#include <canino/core/core.h>
#include <canino/platform/input.h>

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

    // Extraction segura DOD do state contíguo injetado na janela sem Classes 
    CANINO_API const InputState* PlatformGetInputState(Window* window);
    
    // Invocado no fim do frame puramente pra bater os Prev = Current Arrays.
    CANINO_API void PlatformUpdateInputState(Window* window);
    
    // FPS Features Win32
    CANINO_API void PlatformLockCursor(Window* window, bool lock);
    
    // Extrator restrito para Graphics Contexts acoplarem na Plataforma Fisica nativa (Void* Pointer)
    CANINO_API void* PlatformGetNativeWindowHandle(Window* window);
}
