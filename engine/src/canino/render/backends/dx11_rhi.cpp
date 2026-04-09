#include <canino/render/rhi_backend.h>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace canino {

    struct DX11Context {
        ID3D11Device* Device = nullptr;
        ID3D11DeviceContext* DeviceContext = nullptr;
        IDXGISwapChain* SwapChain = nullptr;
        ID3D11RenderTargetView* RenderTargetView = nullptr;
        
        float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    };

    static DX11Context* s_DX11Context = nullptr;

    static bool DX11_Init(void* window) {
        s_DX11Context = new DX11Context();
        HWND hwnd = (HWND)window;
        
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevelArray, 2,
            D3D11_SDK_VERSION, &sd, &s_DX11Context->SwapChain, &s_DX11Context->Device, &featureLevel, &s_DX11Context->DeviceContext) != S_OK)
            return false;

        ID3D11Texture2D* pBackBuffer = nullptr;
        s_DX11Context->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        s_DX11Context->Device->CreateRenderTargetView(pBackBuffer, nullptr, &s_DX11Context->RenderTargetView);
        pBackBuffer->Release();

        std::cout << "[RHI] DirectX11 Inicializado Nativamente com Sucesso!" << std::endl;
        return true;
    }

    static void DX11_Shutdown() {
        if (!s_DX11Context) return;
        if (s_DX11Context->RenderTargetView) s_DX11Context->RenderTargetView->Release();
        if (s_DX11Context->SwapChain) s_DX11Context->SwapChain->Release();
        if (s_DX11Context->DeviceContext) s_DX11Context->DeviceContext->Release();
        if (s_DX11Context->Device) s_DX11Context->Device->Release();
        delete s_DX11Context;
    }

    static void DX11_BeginFrame() {}

    static void DX11_EndFrame() {
        if (s_DX11Context && s_DX11Context->SwapChain)
            s_DX11Context->SwapChain->Present(1, 0); // V-Sync enabled
    }

    static void DX11_SetClearColor(float r, float g, float b, float a) {
        if (s_DX11Context) {
            s_DX11Context->ClearColor[0] = r;
            s_DX11Context->ClearColor[1] = g;
            s_DX11Context->ClearColor[2] = b;
            s_DX11Context->ClearColor[3] = a;
        }
    }

    static void DX11_Clear() {
        if (s_DX11Context && s_DX11Context->DeviceContext && s_DX11Context->RenderTargetView) {
            s_DX11Context->DeviceContext->OMSetRenderTargets(1, &s_DX11Context->RenderTargetView, nullptr);
            s_DX11Context->DeviceContext->ClearRenderTargetView(s_DX11Context->RenderTargetView, s_DX11Context->ClearColor);
        }
    }

    static void DX11_DrawQuad(float, float, float, float, float, float, float) {
        // [WARNING]: Requere construcao de Shaders HLSL e Buffer Maps. Desviado por enquanto conforme Masterplan.
    }

    RHI_VTable GetBackend_DirectX11() {
        RHI_VTable table = {};
        table.Init = DX11_Init;
        table.Shutdown = DX11_Shutdown;
        table.BeginFrame = DX11_BeginFrame;
        table.EndFrame = DX11_EndFrame;
        table.SetClearColor = DX11_SetClearColor;
        table.Clear = DX11_Clear;
        table.DrawQuad = DX11_DrawQuad;
        return table;
    }
}
#endif
