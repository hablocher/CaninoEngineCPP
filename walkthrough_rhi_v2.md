# RHI Multiplexer: O Camaleão de Backends

A estaca que prendia nossa triple-A Engine ao OpenGL acaba de ser estourada e transmutada. A Engine agora escolhe como injetar Fótons na Tela baseada em configuração de memória lida nos primeiros nanossegundos da Inicialização.

## Manobras Táticas Executadas

1. **Parser O(1) de Configuração (`config_parser.cpp`)**:
Criamos um leitor de Custom Data sem uso das engessadas estruturas de `iostream` e `malloc()`. Ele varre blocos de array na RAM e captura formatações de arquivos `.INI` perfeitamente, decidindo qual Backend a main engine deve carregar com base na flag `Backend=`.

2. **O Dispatcher Mestre (`rhi.cpp`)**:
Abolimos a invocação hardcoded em arquivos. A Interface Agnostica RHI agora chama `s_GlobalVTable`. Uma Tabela C-Style explícita de Polimorfismo Físico (Function Pointers) carrega estritamente os drivers requisitados sem perder 1 unico frame em lookup methods ou `virtual` C++ table pointers espalhados. 

3. **O Despertar do Direct3D 11 (`dx11_rhi.cpp`)**:
Deixamos de lado os *DeviceContext* da *WGL* e evocamos os *Componentes COM* da Microsoft através do `D3D11CreateDeviceAndSwapChain()`. Com as lincagens do `d3d11.lib` e `dxgi.lib` cravadas no CMake Root Master, o pipeline nativo do DirectX tomou o controle absoluto do Background Culling.

4. **Stubs DX12 / Vulkan**:
Criamos arquivos bases prontos para serem estendidos, engatilhando os headers de "V-Table" isolados quando chegar o momento de erguermos seus pesados Command Contexts.

## Teste Integrado: Demo1 com DX11!
O output da simulação `Demo1.exe` provou o êxito massivo da substituição. Criamos o `canino.ini` mandando forçar DirectX11 e:

```text
[Sandbox] Iniciando Engine Integral ECS + Multithread...
[RHI Dispatcher] Conectando V-Table Pointers pro DirectX 11.
[RHI] DirectX11 Inicializado Nativamente com Sucesso!
...
```

Nenhum Crash. A RHI conectou silenciosamente as pipelines da API nativa da Microsoft aos RenderCommands abstraídos que outrora batiam no OpenGL! 
