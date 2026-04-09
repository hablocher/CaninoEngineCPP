# Design: Pipeline Programável no DirectX 11 (VBOs & HLSL Shaders)

## Resoluções de Design (Aprovadas)

A Máquina de Shaders será forjada. Ao invés da RHI usar sub-calls nativas lentas p/ desenho (A Grande API Fixa dos anos 90), alocaremos blocos de bytes maciços direto na memória da placa (`VBO/CBO`). E enviaremos apenas equações escalares pros Registers por debaixo dos panos para mudar o shape renderizado.

## Topologia da Arquitetura C-Style DX11
1. **O Motor de Combustão HLSL (Shaders Embededs)**: Scripts puros e hardcoded (em C string) para `Vertex` e `Pixel` shader.
2. **Setup do Contexto Direct3D11**: Passagem de Compilação na Init; Instanciamento do VBO e Constant Buffers atados ao Core Windows nativo.
3. **Mapeamento Primitivo Singular (`DX11_DrawQuad`)**: Implementação 1-to-1 da RHI Agnóstica mapeando diretamente a `Constant Buffer` e enviando os Bytes pro RenderTarget. 
**NOTA**: Batching Massivo O(1) de Rendering Fila não será feito neste update p/ mantermos estabilidade estrutural. DX11 executará via SubDraws isoladas.
