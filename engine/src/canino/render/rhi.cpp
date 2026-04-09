#include <canino/render/rhi.h>
#include <canino/render/rhi_backend.h>
#include <canino/core/config_parser.h>
#include <iostream>
#include <string.h>

namespace canino {

    static RHI_VTable s_GlobalVTable = {};

    bool RHI_Initialize(void* nativeWindowHandle) {
        // Rotina Arquitetural de Selecao de API Graáfica C-Style
        ConfigParser::LoadFromFile("canino.ini");

        char backendName[64] = "OpenGL"; // Default se nao achar
        ConfigParser::GetString("Backend", backendName, sizeof(backendName));

        // Roteamento O(0) String-Check
        if (strcmp(backendName, "DirectX11") == 0) {
            std::cout << "[RHI Dispatcher] Conectando V-Table Pointers pro DirectX 11." << std::endl;
            s_GlobalVTable = GetBackend_DirectX11();
        } 
        else if (strcmp(backendName, "DirectX12") == 0) {
            std::cout << "[RHI Dispatcher] Conectando V-Table Pointers pro DirectX 12." << std::endl;
            s_GlobalVTable = GetBackend_DirectX12();
        }
        else if (strcmp(backendName, "Vulkan") == 0) {
            std::cout << "[RHI Dispatcher] Conectando V-Table Pointers pro Vulkan." << std::endl;
            s_GlobalVTable = GetBackend_Vulkan();
        }
        else {
            std::cout << "[RHI Dispatcher] Conectando V-Table Pointers pro Legacy OpenGL." << std::endl;
            s_GlobalVTable = GetBackend_OpenGL();
        }

        // Sobe o Monstro
        if (s_GlobalVTable.Init) return s_GlobalVTable.Init(nativeWindowHandle);
        return false;
    }

    void RHI_Shutdown() {
        if (s_GlobalVTable.Shutdown) s_GlobalVTable.Shutdown();
    }

    void RHI_BeginFrame() {
        if (s_GlobalVTable.BeginFrame) s_GlobalVTable.BeginFrame();
    }

    void RHI_EndFrame() {
        if (s_GlobalVTable.EndFrame) s_GlobalVTable.EndFrame();
    }

    namespace RenderCommand {
        void SetClearColor(float r, float g, float b, float a) {
            if (s_GlobalVTable.SetClearColor) s_GlobalVTable.SetClearColor(r, g, b, a);
        }

        void Clear() {
            if (s_GlobalVTable.Clear) s_GlobalVTable.Clear();
        }

        void DrawQuad(float x, float y, float w, float h, float r, float g, float b) {
            if (s_GlobalVTable.DrawQuad) s_GlobalVTable.DrawQuad(x, y, w, h, r, g, b);
        }

        void* CreateTexture(const char* filepath, int* width, int* height) {
            if (s_GlobalVTable.CreateTexture) return s_GlobalVTable.CreateTexture(filepath, width, height);
            return nullptr;
        }

        void DrawCube(const Mat4& transform, void* texture) {
            if (s_GlobalVTable.DrawCube) s_GlobalVTable.DrawCube(transform, texture);
        }
    }
}
