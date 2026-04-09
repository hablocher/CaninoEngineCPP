#include <canino/render/rhi_backend.h>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace canino {

    __declspec(align(16)) struct ConstantBufferData {
        float Transform[4]; // x, y, w, h
        float Color[4];     // r, g, b, 1.0f
    };

    struct DX11Context {
        ID3D11Device* Device = nullptr;
        ID3D11DeviceContext* DeviceContext = nullptr;
        IDXGISwapChain* SwapChain = nullptr;
        ID3D11RenderTargetView* RenderTargetView = nullptr;
        
        ID3D11VertexShader* VertexShader = nullptr;
        ID3D11PixelShader* PixelShader = nullptr;
        ID3D11InputLayout* InputLayout = nullptr;
        ID3D11Buffer* VertexBuffer = nullptr;
        ID3D11Buffer* ConstantBuffer = nullptr;

        float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    };

    static DX11Context* s_DX11Context = nullptr;

    const char* g_ShaderSource = R"(
        cbuffer CBO : register(b0) {
            float4 Transform; // x, y, w, h
            float4 Color;     // r, g, b, 1
        };

        struct VS_IN {
            float2 Pos : POSITION;
        };

        struct PS_IN {
            float4 Pos : SV_POSITION;
            float4 Color : COLOR;
        };

        PS_IN VSMain(VS_IN input) {
            PS_IN output;
            output.Pos = float4((input.Pos.x * Transform.z) + Transform.x, (input.Pos.y * Transform.w) + Transform.y, 0.0f, 1.0f);
            output.Color = Color;
            return output;
        }

        float4 PSMain(PS_IN input) : SV_TARGET {
            return input.Color;
        }
    )";

    static bool DX11_Init(void* window) {
        s_DX11Context = new DX11Context();
        HWND hwnd = (HWND)window;
        
        // Setup SwapChain
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

        // ======= SHADER COMPILATION NO RUNTIME ========
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        D3DCompile(g_ShaderSource, strlen(g_ShaderSource), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
        
        ID3DBlob* psBlob = nullptr;
        D3DCompile(g_ShaderSource, strlen(g_ShaderSource), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, nullptr);

        s_DX11Context->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &s_DX11Context->VertexShader);
        s_DX11Context->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &s_DX11Context->PixelShader);

        // Input Layout
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        s_DX11Context->Device->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &s_DX11Context->InputLayout);
        
        vsBlob->Release(); psBlob->Release();
        if (errorBlob) errorBlob->Release();

        // ======= UNIT QUAD VERTEX BUFFER ========
        float vertices[] = {
            0.0f, 1.0f, // Top-Left
            1.0f, 1.0f, // Top-Right
            0.0f, 0.0f, // Bottom-Left
            1.0f, 0.0f  // Bottom-Right
        };
        D3D11_BUFFER_DESC vbd = {};
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.ByteWidth = sizeof(vertices);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA vinitData = {};
        vinitData.pSysMem = vertices;
        s_DX11Context->Device->CreateBuffer(&vbd, &vinitData, &s_DX11Context->VertexBuffer);

        // ======= CONSTANT BUFFER ========
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.ByteWidth = sizeof(ConstantBufferData);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        s_DX11Context->Device->CreateBuffer(&cbd, nullptr, &s_DX11Context->ConstantBuffer);

        // Setup Viewport for native Full Window
        RECT rect;
        GetClientRect(hwnd, &rect);
        D3D11_VIEWPORT vp = {};
        vp.Width = (float)(rect.right - rect.left);
        vp.Height = (float)(rect.bottom - rect.top);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        s_DX11Context->DeviceContext->RSSetViewports(1, &vp);

        // Bind global pipeline states avoiding draw overhead
        s_DX11Context->DeviceContext->IASetInputLayout(s_DX11Context->InputLayout);
        s_DX11Context->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        s_DX11Context->DeviceContext->VSSetShader(s_DX11Context->VertexShader, nullptr, 0);
        s_DX11Context->DeviceContext->PSSetShader(s_DX11Context->PixelShader, nullptr, 0);
        
        UINT stride = sizeof(float) * 2;
        UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &s_DX11Context->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->VSSetConstantBuffers(0, 1, &s_DX11Context->ConstantBuffer);

        std::cout << "[RHI] DirectX11 Shaders Compilation e Hardware VBOs montados no Kernel!" << std::endl;
        return true;
    }

    static void DX11_Shutdown() {
        if (!s_DX11Context) return;
        if (s_DX11Context->ConstantBuffer) s_DX11Context->ConstantBuffer->Release();
        if (s_DX11Context->VertexBuffer) s_DX11Context->VertexBuffer->Release();
        if (s_DX11Context->InputLayout) s_DX11Context->InputLayout->Release();
        if (s_DX11Context->PixelShader) s_DX11Context->PixelShader->Release();
        if (s_DX11Context->VertexShader) s_DX11Context->VertexShader->Release();

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

    static void DX11_DrawQuad(float x, float y, float w, float h, float r, float g, float b) {
        if (!s_DX11Context) return;

        // Mapeamento DOD direto do CBO pra injetar matriz
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
            ConstantBufferData* pData = (ConstantBufferData*)mappedResource.pData;
            pData->Transform[0] = x;
            pData->Transform[1] = y;
            pData->Transform[2] = w;
            pData->Transform[3] = h;
            pData->Color[0] = r;
            pData->Color[1] = g;
            pData->Color[2] = b;
            pData->Color[3] = 1.0f;
            
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer, 0);
        }

        // Taca o sub-draw call pro Buffer (O Shader programável resolve e plota Vertex Array).
        s_DX11Context->DeviceContext->Draw(4, 0);
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
