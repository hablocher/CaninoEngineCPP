#pragma once
#include <canino/core/core.h>

namespace canino {
    
    // Inicialização Agnóstica de Contexto Gráfico de O.S (Recebe aquele vaxamento de HWND)
    CANINO_API bool RHI_Initialize(void* nativeWindowHandle);
    CANINO_API void RHI_Shutdown();

    // Controle de Fluxo Renderizacional de Buffers (DOD Frame Boundaries)
    CANINO_API void RHI_BeginFrame(); 
    CANINO_API void RHI_EndFrame();

    // API brutalista pra submissao de estados de Video Memory. Livre de instancias e V-Tables
    namespace RenderCommand {
        // Envia instrução de Color Masking ao hardware da placa
        CANINO_API void SetClearColor(float r, float g, float b, float a);
        
        // Pura Submissao de bit-clearing pra VRAM
        CANINO_API void Clear();
    }
}
