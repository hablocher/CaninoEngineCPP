# Design: Input Pipeline (Platform Layer)

Na fundação de uma Game Engine orientada a dados (DOD), o estado dos periféricos de *Input* (Teclado e Mouse) são tratados como matrizes processíveis cruas (`[true/false]`) acessáveis estritamente e linearmente no GameLoop por Query direta. O evento é guardado no momento de detecção S.O em vez de ser injetado passivamente.

## Resoluções de Design (Aprovadas)

1. **State Ownership Opaque:** Todo o array do Hardware state de keys e posições morará dentro dos Opaque Pointers da `struct Window`, forçando acesso contextual (`PlatformGetInputState(window)`). 
Isso elimina Singletons Globais massivos que tornariam multithreading e múltiplos clientes futuros (Multi-Window GUI) inviáveis.

## Estrutura do Subsistema

Expandimos a *Platform Layer*:

1. `canino/platform/input.h/cpp` (Novo): Assinaturas Universais de Teclado, Struct `InputState` e APIs de Helpers Stateless ($O(1)$ lookup via array).
2. `canino/platform/window.h`: Adição das queries de leitura do Window state.
3. `canino/platform/windows/window_win32.cpp`: *WindowProc* agressivo lendo Win32 Virtual Key Codes (`VK_SPACE`, `VK_ESCAPE`, Mouses Events) em real-time transmutando pra Matriz Opaque.

## Verification Plan
Os frameworks de Teste nativos receberão blocos validando transições em Delta-Time fictício simulando *IsKeyPressed* e *IsKeyReleased*. A Sandbox demonstrará no Console a leitura perfeita da detecção de Escape forçando fechamento sem vazamentos!
