# Design: Múltiplos Backends RHI e Configuração Externa

A RHI C-Style.

## Resoluções de Design (Aprovadas)
1. **O RHI V-Table (Pointers Base)**: Implementaremos um roteador de Engine em C-Pointers explícito sem qualquer polimorfismo oculto do C++. O Config file ativará as comportas e os ponteiros se trancarão em DX11, DX12, Vulkan ou OpenGL.
2. **Heavy-Backends como Stubs Iniciais**: Para impedir a engine de despencar na complexidade obscura de pipelines de Vulkan de imediato e atrasar a build, os backends de nova geração serão mapeados limpos com `Loaders Stubs`. A prova teórica do dispatcher se baseará no OpenGL funcional pra Demo e no DX11 Color Buffer Swap ativo!

## Topology do The Backend Loader
1. Motor `ConfigParser` de Memory O(1) pra extrair do `.ini` as chaves primárias alocando bytes sem `malloc()`.
2. Extensa quebra da `rhi.h` para `rhi.cpp` controlando o Singleton estático V_Table.
3. Criação dos módulos nativos puristas `dx11_rhi.cpp`, `vulkan_rhi.cpp`, etc, exportando seus callbacks para submissão imediata.

## Caminho Crítico
A Sandbox gerará e lerá `canino.ini` no startup na raiz de sua Working Directory.
