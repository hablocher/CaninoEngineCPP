#include <canino/render/rhi_backend.h>
#include <iostream>

namespace canino {
    static bool Vulkan_Init(void*) {
        std::cout << "[RHI] Vulkan Backend Selecionado (Emulacao Modular). Carregamento futuro de VkInstance ignorado." << std::endl;
        return true;
    }
    static void Vulkan_Shutdown() {}
    static void Vulkan_BeginFrame() {}
    static void Vulkan_EndFrame() {}
    static void Vulkan_SetClearColor(float, float, float, float) {}
    static void Vulkan_Clear() {}
    static void Vulkan_DrawQuad(float, float, float, float, float, float, float) {}
    static void* Vulkan_CreateTexture(const char*, int*, int*) { return nullptr; }
    static void Vulkan_DrawCube(const Mat4&, void*) {}

    RHI_VTable GetBackend_Vulkan() {
        RHI_VTable table = {};
        table.Init = Vulkan_Init;
        table.Shutdown = Vulkan_Shutdown;
        table.BeginFrame = Vulkan_BeginFrame;
        table.EndFrame = Vulkan_EndFrame;
        table.SetClearColor = Vulkan_SetClearColor;
        table.Clear = Vulkan_Clear;
        table.DrawQuad = Vulkan_DrawQuad;
        table.CreateTexture = Vulkan_CreateTexture;
        table.DrawCube = Vulkan_DrawCube;
        return table;
    }
}
