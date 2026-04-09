#pragma once
#include <canino/core/core.h>

// Definição da Estrutura Invisivel de Pointers em Backend C-Style
namespace canino {
    struct RHI_VTable {
        bool (*Init)(void* nativeWindow);
        void (*Shutdown)();
        void (*BeginFrame)();
        void (*EndFrame)();
        void (*SetClearColor)(float, float, float, float);
        void (*Clear)();
        void (*DrawQuad)(float, float, float, float, float, float, float);
    };

    // Declaracoes dos Gateways C++ Nativos pros modulos compilados
    RHI_VTable GetBackend_OpenGL();
    RHI_VTable GetBackend_DirectX11();
    RHI_VTable GetBackend_DirectX12();
    RHI_VTable GetBackend_Vulkan();
}
