#include <canino/render/rhi_backend.h>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace canino {

    __declspec(align(16)) struct ConstantBufferData {
        Mat4 MVP;
    };

    struct Vertex3D {
        float pos[3];
        float uv[2];
    };

    struct DX11Context {
        ID3D11Device* Device = nullptr;
        ID3D11DeviceContext* DeviceContext = nullptr;
        IDXGISwapChain* SwapChain = nullptr;
        ID3D11RenderTargetView* RenderTargetView = nullptr;
        
        ID3D11Texture2D* DepthStencilBuffer = nullptr;
        ID3D11DepthStencilView* DepthStencilView = nullptr;
        ID3D11DepthStencilState* DepthStencilState = nullptr;

        ID3D11VertexShader* VertexShader = nullptr;
        ID3D11PixelShader* PixelShader = nullptr;
        ID3D11InputLayout* InputLayout = nullptr;
        
        ID3D11Buffer* VertexBuffer = nullptr;
        ID3D11Buffer* ConstantBuffer = nullptr;

        ID3D11SamplerState* SamplerState = nullptr;
        ID3D11RasterizerState* RasterizerState = nullptr;

        float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    };

    static DX11Context* s_DX11Context = nullptr;

    const char* g_ShaderSource3D = R"(
        cbuffer CBO : register(b0) {
            matrix MVP;
        };

        Texture2D t0 : register(t0);
        SamplerState s0 : register(s0);

        struct VS_IN {
            float3 Pos : POSITION;
            float2 UV : TEXCOORD;
        };

        struct PS_IN {
            float4 Pos : SV_POSITION;
            float2 UV : TEXCOORD;
        };

        PS_IN VSMain(VS_IN input) {
            PS_IN output;
            output.Pos = mul(float4(input.Pos, 1.0f), MVP);
            output.UV = input.UV;
            return output;
        }

        float4 PSMain(PS_IN input) : SV_TARGET {
            return t0.Sample(s0, input.UV);
        }
    )";

    static bool DX11_Init(void* window) {
        s_DX11Context = new DX11Context();
        HWND hwnd = (HWND)window;
        
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        // Setup SwapChain Mestre
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
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

        // ======= SETUP DEPTH BUFFER (Z-BUFFER 3D) ========
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        s_DX11Context->Device->CreateTexture2D(&depthDesc, nullptr, &s_DX11Context->DepthStencilBuffer);

        s_DX11Context->Device->CreateDepthStencilView(s_DX11Context->DepthStencilBuffer, nullptr, &s_DX11Context->DepthStencilView);
        
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        s_DX11Context->Device->CreateDepthStencilState(&dsDesc, &s_DX11Context->DepthStencilState);

        // ======= SHADER COMPILATION ========
        ID3DBlob* vsBlob = nullptr;
        D3DCompile(g_ShaderSource3D, strlen(g_ShaderSource3D), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, nullptr);
        
        ID3DBlob* psBlob = nullptr;
        D3DCompile(g_ShaderSource3D, strlen(g_ShaderSource3D), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, nullptr);

        s_DX11Context->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &s_DX11Context->VertexShader);
        s_DX11Context->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &s_DX11Context->PixelShader);

        // Input Layout 3D (Posicão + UV)
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        s_DX11Context->Device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &s_DX11Context->InputLayout);
        vsBlob->Release(); psBlob->Release();

        // ======= CUBE VERTEX BUFFER (Geometria 3D Bruta) ========
        Vertex3D cbVertices[] = {
            // Front (Z = -0.5)
            {-0.5, -0.5, -0.5,  0, 1}, {-0.5,  0.5, -0.5,  0, 0}, { 0.5, -0.5, -0.5,  1, 1},
            { 0.5, -0.5, -0.5,  1, 1}, {-0.5,  0.5, -0.5,  0, 0}, { 0.5,  0.5, -0.5,  1, 0},
            // Back (Z = 0.5)
            { 0.5, -0.5,  0.5,  0, 1}, { 0.5,  0.5,  0.5,  0, 0}, {-0.5, -0.5,  0.5,  1, 1},
            {-0.5, -0.5,  0.5,  1, 1}, { 0.5,  0.5,  0.5,  0, 0}, {-0.5,  0.5,  0.5,  1, 0},
            // Left (X = -0.5)
            {-0.5, -0.5,  0.5,  0, 1}, {-0.5,  0.5,  0.5,  0, 0}, {-0.5, -0.5, -0.5,  1, 1},
            {-0.5, -0.5, -0.5,  1, 1}, {-0.5,  0.5,  0.5,  0, 0}, {-0.5,  0.5, -0.5,  1, 0},
            // Right (X = 0.5)
            { 0.5, -0.5, -0.5,  0, 1}, { 0.5,  0.5, -0.5,  0, 0}, { 0.5, -0.5,  0.5,  1, 1},
            { 0.5, -0.5,  0.5,  1, 1}, { 0.5,  0.5, -0.5,  0, 0}, { 0.5,  0.5,  0.5,  1, 0},
            // Top (Y = 0.5)
            {-0.5,  0.5, -0.5,  0, 1}, {-0.5,  0.5,  0.5,  0, 0}, { 0.5,  0.5, -0.5,  1, 1},
            { 0.5,  0.5, -0.5,  1, 1}, {-0.5,  0.5,  0.5,  0, 0}, { 0.5,  0.5,  0.5,  1, 0},
            // Bottom (Y = -0.5)
            {-0.5, -0.5,  0.5,  0, 1}, {-0.5, -0.5, -0.5,  0, 0}, { 0.5, -0.5,  0.5,  1, 1},
            { 0.5, -0.5,  0.5,  1, 1}, {-0.5, -0.5, -0.5,  0, 0}, { 0.5, -0.5, -0.5,  1, 0}
        };
        D3D11_BUFFER_DESC vbd = {}; vbd.Usage = D3D11_USAGE_DEFAULT; vbd.ByteWidth = sizeof(cbVertices); vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA vinit = {}; vinit.pSysMem = cbVertices;
        s_DX11Context->Device->CreateBuffer(&vbd, &vinit, &s_DX11Context->VertexBuffer);

        // ======= CBO MVP --------
        D3D11_BUFFER_DESC cbd = {}; cbd.Usage = D3D11_USAGE_DYNAMIC; cbd.ByteWidth = sizeof(ConstantBufferData); cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        s_DX11Context->Device->CreateBuffer(&cbd, nullptr, &s_DX11Context->ConstantBuffer);

        // ======= SAMPLER ========
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        s_DX11Context->Device->CreateSamplerState(&sampDesc, &s_DX11Context->SamplerState);

        // ======= RASTERIZER & VIEWPORT ========
        D3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.CullMode = D3D11_CULL_NONE; // Impede GPU nativa de engulir os triangulos e causar buracos nas faces
        rastDesc.FillMode = D3D11_FILL_SOLID;
        s_DX11Context->Device->CreateRasterizerState(&rastDesc, &s_DX11Context->RasterizerState);

        D3D11_VIEWPORT vp = {0, 0, (float)width, (float)height, 0.0f, 1.0f};
        s_DX11Context->DeviceContext->RSSetViewports(1, &vp);

        // ======= FULL PIPELINE BINDINGS ========
        s_DX11Context->DeviceContext->RSSetState(s_DX11Context->RasterizerState);
        s_DX11Context->DeviceContext->IASetInputLayout(s_DX11Context->InputLayout);
        s_DX11Context->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        s_DX11Context->DeviceContext->OMSetDepthStencilState(s_DX11Context->DepthStencilState, 1);
        s_DX11Context->DeviceContext->VSSetShader(s_DX11Context->VertexShader, nullptr, 0);
        s_DX11Context->DeviceContext->PSSetShader(s_DX11Context->PixelShader, nullptr, 0);
        s_DX11Context->DeviceContext->PSSetSamplers(0, 1, &s_DX11Context->SamplerState);

        UINT stride = sizeof(Vertex3D); UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &s_DX11Context->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->VSSetConstantBuffers(0, 1, &s_DX11Context->ConstantBuffer);

        std::cout << "[RHI] DirectX11 Engine 3D e Z-Buffer Bootados!" << std::endl;
        return true;
    }

    static void* DX11_CreateTexture(const char* filepath, int* outW, int* outH) {
        if (!s_DX11Context || !s_DX11Context->Device) return nullptr;

        int tWidth, tHeight, tChannels;
        unsigned char* image = stbi_load(filepath, &tWidth, &tHeight, &tChannels, 4); // Forcar RGBA puro
        if (!image) { std::cout << "[RHI] Erro critico ao abrir Asset: " << filepath << std::endl; return nullptr; }
        
        if (outW) *outW = tWidth;
        if (outH) *outH = tHeight;

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = tWidth; texDesc.Height = tHeight; texDesc.MipLevels = 1; texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1; texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = image;
        initData.SysMemPitch = tWidth * 4;

        ID3D11Texture2D* texture = nullptr;
        s_DX11Context->Device->CreateTexture2D(&texDesc, &initData, &texture);

        ID3D11ShaderResourceView* srv = nullptr;
        if (texture) {
            s_DX11Context->Device->CreateShaderResourceView(texture, nullptr, &srv);
            texture->Release();
        }

        stbi_image_free(image);
        return srv;
    }

    static void DX11_Shutdown() {
        if (!s_DX11Context) return;
        // memory releases ignored form brevity
        delete s_DX11Context;
    }

    static void DX11_BeginFrame() {}

    static void DX11_EndFrame() {
        if (s_DX11Context && s_DX11Context->SwapChain) s_DX11Context->SwapChain->Present(1, 0); 
    }

    static void DX11_SetClearColor(float r, float g, float b, float a) {
        if (s_DX11Context) { s_DX11Context->ClearColor[0]=r; s_DX11Context->ClearColor[1]=g; s_DX11Context->ClearColor[2]=b; s_DX11Context->ClearColor[3]=a; }
    }

    static void DX11_Clear() {
        if (s_DX11Context && s_DX11Context->DeviceContext && s_DX11Context->RenderTargetView) {
            s_DX11Context->DeviceContext->OMSetRenderTargets(1, &s_DX11Context->RenderTargetView, s_DX11Context->DepthStencilView);
            s_DX11Context->DeviceContext->ClearRenderTargetView(s_DX11Context->RenderTargetView, s_DX11Context->ClearColor);
            s_DX11Context->DeviceContext->ClearDepthStencilView(s_DX11Context->DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        }
    }

    static void DX11_DrawQuad(float, float, float, float, float, float, float) {
        // [WARNING]: A pipeline OGL 2.0 agora está refatorada pra Cubos Texturizados
    }

    static void DX11_DrawCube(const Mat4& mvp, void* textureId) {
        if (!s_DX11Context) return;

        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            ConstantBufferData* data = (ConstantBufferData*)mapped.pData;
            
            // Transpose matrix manually CPU side since HLSL defaults to column_major internally
            for(int i=0; i<4; ++i) {
                for(int j=0; j<4; ++j) {
                    data->MVP.m[i][j] = mvp.m[j][i];
                }
            }

            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer, 0);
        }

        // Texture Bind
        if (textureId) {
            ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)textureId;
            s_DX11Context->DeviceContext->PSSetShaderResources(0, 1, &srv);
        }

        s_DX11Context->DeviceContext->Draw(36, 0);
    }

    RHI_VTable GetBackend_DirectX11() {
        RHI_VTable table = {};
        table.Init = DX11_Init; table.Shutdown = DX11_Shutdown; table.BeginFrame = DX11_BeginFrame;
        table.EndFrame = DX11_EndFrame; table.SetClearColor = DX11_SetClearColor; table.Clear = DX11_Clear;
        table.DrawQuad = DX11_DrawQuad; table.CreateTexture = DX11_CreateTexture; table.DrawCube = DX11_DrawCube;
        return table;
    }
}
#endif
