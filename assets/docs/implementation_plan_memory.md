# Design: Sistema de Memory Management (Arenas & Pools)

No paradigma Data-Oriented e em Engines de alta performance, `new` e `delete` indiscriminados no loop principal (`hot path`) destroem a performance através de *cache misses* e *heap fragmentation*, fragmentando completamente o sistema operacional. Como o arquiteto da engine, meu objetivo primário é restringirmos o uso da memória heap do SO para **tempos de carga sistêmica (Load Time)**, alocando grandes blocos de forma agressiva. 

Em todo momento no jogo e *runtime*, iremos rotear *sub-alocações* dentro desses blocos gerenciados por nós usando dois mecanismos incrivelmente simples e cirúrgicos: **Linear Arenas** e **Fixed-size Pools**.

## Resoluções de Design (Aprovadas)

1. **Backing Allocator:** Opção A (`std::malloc`). Inicialmente usaremos abstração padrão C++ para o backend contíguo da Arena e do Pool, por ser isolado o suficiente e portátil.
2. **Alinhamento Padrão:** Adotado firmemente o espaçamento base de **8 bytes** para garantir alinhamento *cache-friendly* em todas as transformações de structs fundamentais via Arenas.

## Topologia Proposta

Desenvolveremos nossa biblioteca primária de Memória em `canino/core`. Ela trará classes puras, sem herança (sem `class IAllocator`), resolvendo tudo através de structs compactos e simples.

### 1. The Linear Allocator (`MemoryArena`)
O alocador de complexidade $O(1)$. Funciona apenas deslizando um cursor de *offset* em cima do bloco previamente alocado. Não existe `free()`, a *Arena* inteira é limpa em um pulso zerando seu offset (exemplo: Frame Arenas ou Level Arenas). 

### 2. The Fixed-Size Allocator (`MemoryPool`)
Baseado em uma *Free List* embutida na própria memória. O bloco é fatiado em dezenas de pedaços de mesmo tamanho e cada "nó" não utilizado aponta para o próximo livre. Liberação unificada ideal para instâncias e remoções de Entidades (ECS).

## Verification Plan
O benchmark será efetuado executando 1 milhão de alocações via Heap do SO (*new/delete* comum array-oriented) e 1 milhão pela nossa Linear Arena implementada localmente em `sandbox/src/main.cpp`.
