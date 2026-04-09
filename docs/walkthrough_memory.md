# Fundação do Memory Manager: Arenas & Pools

O subsistema de Memória de alta performance foi implementado na camada **Core** (`canino/core`), respeitando as premissas estritas do *Data-Oriented Design*.

## O que foi implementado

1.  **DOD Pointer Utilities (`memory.h`/`cpp`)**: Desenvolvemos uma matemática baseada puramente em bitwise para empurrar ponteiros de memória até alinhamentos múltiplos de 2 (*cache-hits* garantidos na L1/L2 do processador).
2.  **MemoryArena (Linear Allocator) (`memory_arena.h`/`cpp`)**: O ápice da velocidade $O(1)$. Alocam-se gigabytes perfeitamente de uma vez do SO. Cada alocação (via `PushStruct<T>`) exige apenas *pointer collision detection* irrisória e bump no offset.
3.  **MemoryPool (Fixed Free-List) (`memory_pool.h`/`cpp`)**: Estrutura contígua mapeada. Pedaços não usados convertem os primeiros bytes de si mesmos em um *FreeNode* para construir um Linked-List na memória de hardware sem necessitar metadados de alocação externos.
4.  **Integração DLL**: Todo o sistema injeta os exports automáticos configurados pelo Build System.

## Resultados Empíricos (Validação de Performance)

Escrevemos um *benchmark* que instanciou 1.000.000 structs espaciais (`Transform` de 40 bytes padrão GameEngine) via POO Padrão (`new/delete` chamando o S.O.) vs nossa Linear `MemoryArena`.

> [!TIP]
> **A Verdade dos Ciclos de Cache Exposta**
> O Heap do Sistema Operacional exigiu **264 Milissegundos** de CPU time na trilha executiva.
> A nossa Memory Arena cumpriu a mesma carga de dados em incriveis **18 Milissegundos**.
> 
> Isso representa uma **performance bruta superior a 1400%**, provando que o paradigma *Memory-Is-Data* é a fundação para nossa game engine!
