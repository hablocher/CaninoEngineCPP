# O.S Input Pipeline: Mapeamento em Alta Rotatividade (*Data-Oriented*)

O mapeamento de Hardware (Mouse e Keys) na `Canino Engine` agora lê puramente matrizes booleanas alocadas no OS Header. Isolamos a Poluição da API do Windows garantindo alta rotatividade $O(1)$ pra dentro da Engine.

## O que foi implementado

1.  **State Contíguo Sem Globais (`input.h`)**: Construímos a `struct InputState`. Diferente das grandes engines, nossos estados de clique de mouse ou pressionamento de letra não são Globais (*Singletons* não existem aqui). A matriz de bytes inteira é estritamente de propriedade contextual do *Opaque Pointer* daquela Janela instanciada.
2.  **API Stateless Rápida**: Foram adicionadas queries em C-Style funcional como `IsKeyPressed(state, CANINO_KEY_SPACE)` que comparam a Matrix do Frame Corrente contra a Matrix Fixada Latch do Frame Anterior num intervalo O(1) de custo irrisório, avaliando transições (KeyDown, KeyUp, IsHeld).
3.  **Kernel Interception Loop (`window_win32.cpp`)**: A interceptação foi forjada no `WindowProc`. Mensagens viscerais GDI (como `WM_KEYDOWN` ou `WM_MOUSEMOVE` com coordenadas hi-res bit-shifted de `lParam`) alimentam a struct sem disparar instâncias ou Callbacks Virtuais (`Listeners` mortos).
4.  **Sandbox Tooling Mode**: A branch principal de teste e nosso Loop de `main.cpp` ganharam detecções de interrupção forçada (`Key: ESCAPE`), tiros ficcionais de mouse (Click-Esquerdo), e WASD. 
5.  **CTest Validations**: A engine Unit test framework provou não só que as structs funcionam sob stress temporal e mudança frame a frame de bits, como re-avaliou e manteve a flag `100% PASS` do CMake Test.

## Benefícios Arquiteturais de Curto Prazo (Cache-Ready)

Nosso ECS quando construído não precisará adicionar dezenas de `eventHandlers` nas entidades controladas pelos jogadores. Os sistemas de lógica poderão importar as Matrizes da UI diretamente pro escopo e varrê-las paralelamente, consumindo o mínimo de RAM e aproveitando-se 100% de linhas de Cache do Processador (Zero Pointer Hopping em Classes OO).
