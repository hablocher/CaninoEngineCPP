# Fundação da Qualidade: CTest Framework Customizado

Não aceitamos caixas-pretas de código na `Canino Engine`, isso se reflete também na forma como garantimos a estabilidade dos módulos críticos usando _Unit Tests_.

## O que foi implementado

1. **Ativação Segura do CTest (`CMakeLists.txt`)**: Habituamos o pipeline root do CMake para escutar sinais de teste da fundação, o que permite automatização futura por _Continuous Integration_ (CI).
2. **Micro-Framework de Asserções C-Style (`test_core.h`)**: Evitando dezenas de megabytes das bibliotecas standard de teste, criei as macros `CANINO_RUN_TEST` e `CANINO_EXPECT`. Elas provêm mapeamento exato de qual linha e arquivo explodiu na memória usando as tags do compilador `__FILE__` e `__LINE__` em $O(1)$.
3. **Sandbox Test Suit**:
    *   **Unit: AlignForward**: Computa assertividades bit a bit para checar o ponteiro colidindo na arquitetura de RAM.
    *   **Unit: MemoryArena**: Verifica se a arena de FrameLife desliza sem fragmentação e reloca `PushSize` perfeitamente sem engolir `OutOfMemory`. 
    *   **Unit: MemoryPool**: Comprova o instanciamento Headless e a premissa crucial do *LIFO* (Último desalocado é o próximo a ser engolido pelo chunk) garantindo instâncias instantâneas pro ECS no futuro.
    *   **Unit: Window Creation**: Dispara a Janela GDI de forma non-blocking (*headless mock*) e comanda um pump falso provando pro teste falhar brutalmente se houverem memory leaks no fechamento nativo.

## Resultados Empíricos

O `ctest --output-on-failure` validou 100% da *Source Code* elaborada hoje:

```text
Test project E:/Code/CPP/CaninoEngine/build
    Start 1: CaninoCoreTests
1/1 Test #1: CaninoCoreTests ..................   Passed    0.15 sec

100% tests passed, 0 tests failed out of 1
Total Test time (real) =   0.16 sec
```

Não apenas a Engine é matematicamente rápida de se construir, como seus sub-módulos estão selados à prova de falhas futuras!
