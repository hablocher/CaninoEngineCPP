# The ECS Core: Entidades vazias e o Triunfo DOD

A transmutação da *Canino Engine* de C++ Orientado a Objeto para **Data-Oriented** está finalizada em seu coração lógico. Decretamos a morte da palavra chave `class Player : GameObject`. Um *"GameObject"* agora pesa irrisórios `32-bits` e não aloca NADA no Heap.

## Técnicas Brutais Implementadas

1. **A Extinção do RTTI (`__FUNCSIG__`)**:
Banimos o inchado e lento Dynamic-Casting e bibliotecas nativas de tipagem C++ (`typeid`). Usamos os identificadores puros do Compilador em pré-compilação: `CTE_HashString()`. Isso significa que se você pedir `registry.GetComponent<Transform>()`, a conversão do Tipo do Componente para um Inteiro Assinante custa `0 clocks` na CPU (Resolvido antes do programa inicializar), superando falhas clássicas de limites de DLL da Indústria.
2. **The Sparse Sets (`ComponentArray<T>`)**:
Quando um zumbi morre, seu sub-componente "Posição" no meio da array de milhares de inimigos é marcado pra Deleção. O comando **Swap-And-Pop** arranca o elemento mais distante e tampa o buraco, atualizando os *Pointers*. Iterar por Posicionamentos para renderizar na Tela agora queima as linhas de `L1 Cache` sequencialmente. O processador nem precisa parar pra respirar.

## Consolidação Final (Sandbox)
O arquivo `main.cpp` do Sandbox deixou de gerir cor primitiva hardcoded. Construimos a entidade `player`, demos à ela o componente de teste `CameraTarget(r,g,b)`, anexamos ao **Registry ECS** e agora nós buscamos ele ativamente de dentro da Array O(1) pra sofrer mutação pelo Input.

```text
[TEST] Rodando Test_ECS_RegistryTicking...
[TEST] Rodando Test_ECS_SwapAndPop...
[PASSOU] Todos os sistemas e endereços da Canino Engine testados com sucesso absoluto.
```

O Registry funciona, o Swap and Pop não fragmenta ponteiros, o Loop C-test O(1) de Entidades roda com glória perfeita.
Nossa base sólida Triple A Minimalista está completa.
