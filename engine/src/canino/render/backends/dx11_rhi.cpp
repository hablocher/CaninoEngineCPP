#include <canino/render/rhi_backend.h>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace canino {

    __declspec(align(16)) struct CBOMatrices {
        Mat4 MVP;
    };

    __declspec(align(16)) struct CBOMaterials {
        int UseTexture[4];
        float SolidColor[4];
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
        ID3D11Buffer* ConstantBuffer_Matrices = nullptr;
        ID3D11Buffer* ConstantBuffer_Materials = nullptr;

        ID3D11SamplerState* SamplerState = nullptr;
        ID3D11RasterizerState* RasterizerState = nullptr;

        float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

        struct NativeMesh {
            ID3D11Buffer* VertexBuffer = nullptr;
            ID3D11Buffer* IndexBuffer = nullptr;
            UINT IndexCount = 0;
        };

        std::vector<NativeMesh*> TrackedMeshes;
        std::vector<ID3D11ShaderResourceView*> TrackedTextures;
    };

    static DX11Context* s_DX11Context = nullptr;

    const char* g_ShaderSource3D = R"(
        cbuffer CBO_Matrices : register(b0) {
            matrix MVP;
        };

        cbuffer CBO_Materials : register(b1) {
            int4 UseTexture;
            float4 SolidColor;
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
            if (UseTexture.x == 1) {
                float4 color = t0.Sample(s0, input.UV);
                return float4(color.rgb, 1.0f);
            } else {
                return float4(SolidColor.rgb, 1.0f);
            }
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
        ID3DBlob* errBlob = nullptr;
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;
        
        HRESULT vr = D3DCompile(g_ShaderSource3D, strlen(g_ShaderSource3D), nullptr, nullptr, nullptr, "VSMain", "vs_4_0", 0, 0, &vsBlob, &errBlob);
        if (FAILED(vr)) {
            if (errBlob) std::cout << "[RHI FATAL ERROR] Vertex Shader fail: " << (char*)errBlob->GetBufferPointer() << std::endl;
        }
        HRESULT pr = D3DCompile(g_ShaderSource3D, strlen(g_ShaderSource3D), nullptr, nullptr, nullptr, "PSMain", "ps_4_0", 0, 0, &psBlob, &errBlob);
        if (FAILED(pr)) {
            if (errBlob) std::cout << "[RHI FATAL ERROR] Pixel Shader fail: " << (char*)errBlob->GetBufferPointer() << std::endl;
        }

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
        RenderCommand::Vertex3D cbVertices[] = {
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
        D3D11_BUFFER_DESC cbd0 = {}; cbd0.Usage = D3D11_USAGE_DYNAMIC; cbd0.ByteWidth = sizeof(CBOMatrices); cbd0.BindFlags = D3D11_BIND_CONSTANT_BUFFER; cbd0.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        s_DX11Context->Device->CreateBuffer(&cbd0, nullptr, &s_DX11Context->ConstantBuffer_Matrices);

        D3D11_BUFFER_DESC cbd1 = {}; cbd1.Usage = D3D11_USAGE_DYNAMIC; cbd1.ByteWidth = sizeof(CBOMaterials); cbd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER; cbd1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        s_DX11Context->Device->CreateBuffer(&cbd1, nullptr, &s_DX11Context->ConstantBuffer_Materials);

        // ======= SAMPLER ========
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        sampDesc.MaxAnisotropy = 1;
        
        HRESULT hr = s_DX11Context->Device->CreateSamplerState(&sampDesc, &s_DX11Context->SamplerState);
        if (FAILED(hr)) {
            std::cout << "[RHI FATAL ERROR] CreateSamplerState falhou com código: " << std::hex << hr << std::endl;
        } else {
            std::cout << "[RHI] SamplerState criado e vinculado!" << std::endl;
        }

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

        UINT stride = sizeof(RenderCommand::Vertex3D); UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &s_DX11Context->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->VSSetConstantBuffers(0, 1, &s_DX11Context->ConstantBuffer_Matrices);
        s_DX11Context->DeviceContext->PSSetConstantBuffers(1, 1, &s_DX11Context->ConstantBuffer_Materials);


        std::cout << "[RHI] DirectX11 Engine 3D e Z-Buffer Bootados!" << std::endl;
        return true;
    }

    static void* DX11_CreateTexture(const char* filepath, int* outW, int* outH) {
        if (!s_DX11Context || !s_DX11Context->Device) return nullptr;

        int tWidth, tHeight, tChannels;
        unsigned char* image = stbi_load(filepath, &tWidth, &tHeight, &tChannels, 4); // Forcar RGBA puro
        if (!image) { std::cout << "[RHI] Erro critico ao abrir Asset: " << filepath << std::endl; return nullptr; }
        
        std::cout << "[STBI DEBUG] Lidos " << tWidth << "x" << tHeight << " com " << tChannels << " canais em " << filepath << std::endl;
        std::cout << "[STBI DEBUG PIXELS] " << (int)image[0] << "," << (int)image[1] << "," << (int)image[2] << " - " << (int)image[3] << std::endl;

        if (outW) *outW = tWidth;
        if (outH) *outH = tHeight;

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = tWidth; texDesc.Height = tHeight; 
        texDesc.MipLevels = 1; // Voltaremos ao padrão robusto linear
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1; texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        // O Segredo Negro do D3D11: Se o SlicePitch (DepthPitch) for 0, o Driver pode copiar ZERO bytes dependendo do Vendor!
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = image;
        initData.SysMemPitch = tWidth * 4;
        initData.SysMemSlicePitch = tWidth * tHeight * 4; // <--- O SALVADOR DO UNIVERSO

        ID3D11Texture2D* texture = nullptr;
        s_DX11Context->Device->CreateTexture2D(&texDesc, &initData, &texture);

        ID3D11ShaderResourceView* srv = nullptr;
        if (texture) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = texDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = 1; 
            
            HRESULT shr = s_DX11Context->Device->CreateShaderResourceView(texture, &srvDesc, &srv);
            if (FAILED(shr)) std::cout << "[RHI FATAL] Criar ShaderResourceView falhou: " << std::hex << shr << std::endl;
            texture->Release();
        } else {
            std::cout << "[RHI FATAL] CreateTexture2D FALHOU! Texture estava NULL." << std::endl;
        }

        stbi_image_free(image);
        if (srv) {
            s_DX11Context->TrackedTextures.push_back(srv);
        }
        return srv;
    }

    static void DX11_Shutdown() {
        if (!s_DX11Context) return;
        
        for (auto srv : s_DX11Context->TrackedTextures) {
            if (srv) srv->Release();
        }
        for (auto mesh : s_DX11Context->TrackedMeshes) {
            if (mesh->VertexBuffer) mesh->VertexBuffer->Release();
            if (mesh->IndexBuffer) mesh->IndexBuffer->Release();
            delete mesh;
        }

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

        D3D11_MAPPED_SUBRESOURCE mapped0;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Matrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped0))) {
            CBOMatrices* data0 = (CBOMatrices*)mapped0.pData;
            for(int i=0; i<4; ++i) {
                for(int j=0; j<4; ++j) {
                    data0->MVP.m[i][j] = mvp.m[j][i];
                }
            }
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Matrices, 0);
        }

        D3D11_MAPPED_SUBRESOURCE mapped1;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Materials, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped1))) {
            CBOMaterials* data1 = (CBOMaterials*)mapped1.pData;
            memset(data1, 0, sizeof(CBOMaterials));
            if (textureId) {
                data1->UseTexture[0] = 1;
                data1->SolidColor[0] = 1.0f; data1->SolidColor[1] = 1.0f; data1->SolidColor[2] = 1.0f; data1->SolidColor[3] = 1.0f;
            } else {
                data1->UseTexture[0] = 0;
                data1->SolidColor[0] = 0.5f; data1->SolidColor[1] = 0.5f; data1->SolidColor[2] = 0.5f; data1->SolidColor[3] = 1.0f;
            }
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Materials, 0);
        }

        // Texture Bind
        if (textureId) {
            ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)textureId;
            s_DX11Context->DeviceContext->PSSetShaderResources(0, 1, &srv);
        }

        // Re-bind do Cubo Global (pois DrawMesh sobrescreve a state machine na frame anterior)
        UINT stride = sizeof(RenderCommand::Vertex3D);
        UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &s_DX11Context->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

        s_DX11Context->DeviceContext->Draw(36, 0);
    }

    static void DX11_DrawCubeSolid(const Mat4& mvp, float r, float g, float b) {
        if (!s_DX11Context) return;

        D3D11_MAPPED_SUBRESOURCE mapped0;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Matrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped0))) {
            CBOMatrices* data0 = (CBOMatrices*)mapped0.pData;
            for(int i=0; i<4; ++i) {
                for(int j=0; j<4; ++j) {
                    data0->MVP.m[i][j] = mvp.m[j][i];
                }
            }
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Matrices, 0);
        }

        D3D11_MAPPED_SUBRESOURCE mapped1;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Materials, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped1))) {
            CBOMaterials* data1 = (CBOMaterials*)mapped1.pData;
            memset(data1, 0, sizeof(CBOMaterials));
            data1->SolidColor[0] = r; data1->SolidColor[1] = g; data1->SolidColor[2] = b; data1->SolidColor[3] = 1.0f;
            data1->UseTexture[0] = 0;
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Materials, 0);
        }

        // Re-bind do Cubo Global
        UINT stride = sizeof(RenderCommand::Vertex3D);
        UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &s_DX11Context->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

        s_DX11Context->DeviceContext->Draw(36, 0);
    }

    static void* DX11_CreateMesh(const RenderCommand::Vertex3D* vertices, size_t vCount, const unsigned int* indices, size_t iCount) {
        if (!s_DX11Context) return nullptr;

        DX11Context::NativeMesh* mesh = new DX11Context::NativeMesh();
        mesh->IndexCount = (UINT)iCount;

        // --- Vertex Buffer ---
        D3D11_BUFFER_DESC vDesc = {};
        vDesc.Usage = D3D11_USAGE_DEFAULT;
        vDesc.ByteWidth = sizeof(RenderCommand::Vertex3D) * (UINT)vCount;
        vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA vData = {};
        vData.pSysMem = vertices;
        s_DX11Context->Device->CreateBuffer(&vDesc, &vData, &mesh->VertexBuffer);

        // --- Index Buffer ---
        D3D11_BUFFER_DESC iDesc = {};
        iDesc.Usage = D3D11_USAGE_DEFAULT;
        iDesc.ByteWidth = sizeof(unsigned int) * (UINT)iCount;
        iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA iData = {};
        iData.pSysMem = indices;
        s_DX11Context->Device->CreateBuffer(&iDesc, &iData, &mesh->IndexBuffer);

        s_DX11Context->TrackedMeshes.push_back(mesh);
        return mesh;
    }

    static void DX11_DrawMesh(void* meshHandle, const Mat4& mvp, void* textureId) {
        if (!s_DX11Context || !meshHandle) return;
        DX11Context::NativeMesh* mesh = (DX11Context::NativeMesh*)meshHandle;

        D3D11_MAPPED_SUBRESOURCE mapped0;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Matrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped0))) {
            CBOMatrices* data0 = (CBOMatrices*)mapped0.pData;
            for(int i=0; i<4; ++i) {
                for(int j=0; j<4; ++j) {
                    data0->MVP.m[i][j] = mvp.m[j][i];
                }
            }
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Matrices, 0);
        }

        D3D11_MAPPED_SUBRESOURCE mapped1;
        if (SUCCEEDED(s_DX11Context->DeviceContext->Map(s_DX11Context->ConstantBuffer_Materials, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped1))) {
            CBOMaterials* data1 = (CBOMaterials*)mapped1.pData;
            memset(data1, 0, sizeof(CBOMaterials));
            if (textureId) {
                data1->UseTexture[0] = 1;
                data1->SolidColor[0] = 1.0f; data1->SolidColor[1] = 1.0f; data1->SolidColor[2] = 1.0f; data1->SolidColor[3] = 1.0f; 
            } else {
                data1->SolidColor[0] = 0.5f; data1->SolidColor[1] = 0.5f; data1->SolidColor[2] = 0.5f; data1->SolidColor[3] = 1.0f;
                data1->UseTexture[0] = 0;
            }
            s_DX11Context->DeviceContext->Unmap(s_DX11Context->ConstantBuffer_Materials, 0);
        }

        if (textureId) {
            ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)textureId;
            s_DX11Context->DeviceContext->PSSetShaderResources(0, 1, &srv);
        }

        // BIND Mesh Buffers e Draw Indexed
        UINT stride = sizeof(RenderCommand::Vertex3D);
        UINT offset = 0;
        s_DX11Context->DeviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &stride, &offset);
        s_DX11Context->DeviceContext->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        
        s_DX11Context->DeviceContext->DrawIndexed(mesh->IndexCount, 0, 0);
    }

    RHI_VTable GetBackend_DirectX11() {
        RHI_VTable table = {};
        table.Init = DX11_Init; table.Shutdown = DX11_Shutdown; table.BeginFrame = DX11_BeginFrame;
        table.EndFrame = DX11_EndFrame; table.SetClearColor = DX11_SetClearColor; table.Clear = DX11_Clear;
        table.DrawQuad = DX11_DrawQuad; table.CreateTexture = DX11_CreateTexture; table.DrawCube = DX11_DrawCube;
        table.DrawCubeSolid = DX11_DrawCubeSolid;
        table.CreateMesh = DX11_CreateMesh;
        table.DrawMesh = DX11_DrawMesh;
        return table;
    }
}
#endif
