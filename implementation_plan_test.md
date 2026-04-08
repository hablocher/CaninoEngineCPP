# Design: Sistema de Unit Tests (Zero-Bloat)

Introduzir testes unitários automatizados é essencial para garantir a sanidade da C++ Engine a longo prazo, garantindo que refatorações em DOD não quebrem o gerenciamento estrito da memória.

## Resoluções de Design (Aprovadas)

1. **Framework Próprio Leve:** Recusamos inteiramente injetar monstruosidades de dependências corporativas como GoogleTest, Doctest ou BoostTest no projeto. Escreveremos nossas próprias macros C-Style brutas em `test_core.h` para garantir a funcionalidade em tempo irrisório, ligadas aos Targets pass-fail absolutos de Pipeline do Ninja/Make via integração natural do `CTest`.

## Estrutura do Test Engine

```text
CaninoEngine/
├── CMakeLists.txt         (Injetado include(CTest))
└── tests/                 
    ├── CMakeLists.txt     (Target CaninoTests)
    └── src/
        ├── test_core.h    (Macros CANINO_EXPECT e CANINO_RUN_TEST)
        ├── test_main.cpp  (Entrada O(N) dos pipelines)
        ├── test_memory.cpp (Garante LIFO no Pool, OOM na Arena)
        └── test_platform.cpp (Cria e mata Window sem OS leaks)
```

## Verification Plan

Realizaremos o Build total e rodaremos puramente os executaveis nativos integrados. Seu Console reportara `100% tests passed` do injetor do CMake e garantira a fundação ate aqui finalizada com estabilidade.
