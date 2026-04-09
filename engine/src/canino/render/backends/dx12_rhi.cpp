#include <canino/render/rhi_backend.h>
#include <iostream>

namespace canino {
    static bool DX12_Init(void*) {
        std::cout << "[RHI] DirectX 12 Backend Selecionado (Emulacao Modular). Pipeline de Filas ignorado." << std::endl;
        return true;
    }
    static void DX12_Shutdown() {}
    static void DX12_BeginFrame() {}
    static void DX12_EndFrame() {}
    static void DX12_SetClearColor(float, float, float, float) {}
    static void DX12_Clear() {}
    static void DX12_DrawQuad(float, float, float, float, float, float, float) {}
    static void* DX12_CreateTexture(const char*, int*, int*) { return nullptr; }
    static void DX12_DrawCube(const Mat4&, void*) {}

    RHI_VTable GetBackend_DirectX12() {
        RHI_VTable table = {};
        table.Init = DX12_Init;
        table.Shutdown = DX12_Shutdown;
        table.BeginFrame = DX12_BeginFrame;
        table.EndFrame = DX12_EndFrame;
        table.SetClearColor = DX12_SetClearColor;
        table.Clear = DX12_Clear;
        table.DrawQuad = DX12_DrawQuad;
        table.CreateTexture = DX12_CreateTexture;
        table.DrawCube = DX12_DrawCube;
        return table;
    }
}
