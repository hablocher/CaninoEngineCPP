# Design: Rendering Hardware Interface (RHI)

O Santo Graal de qualquer Engine é a camada de Renderização. A **Rendering Hardware Interface (RHI)** servirá como nossa barreira de isolamento: Command Buffers abstratos, Swapchains abstratos. A RHI atuará como um proxy DOD com ponteiros para backend enxutos.

## Resoluções de Design (Aprovadas)

1. **Opção B: Alicerce OpenGL:** Para fins de _Stepping Stone_ de arquitetamento sem necessitarmos puxar o gigantesco Vulkan SDK, implementaremos a RHI DOD agnóstica injetando por trás das cortinas chamadas brutas a `opengl32.lib` natural do Windows nativo. Faremos a troca pra Vulkan no momento oportuno.
2. **HWND Leak:** Invadiremos sutil e de forma explícita o Native Window Handle atravessando a barreira pra plugar o Ponto Flutuante GDI ao _WGL Graphics Context_.

## Topologia Proposta da RHI

Expandiremos a camada `canino/render/`:

### 1. The Frontend Core (`rhi.h`)
Onde todo handle cru sobrevive e API agnóstica nasce.
- `RHI_Initialize()`
- `RenderCommand::Clear()` 

### 2. The Opaque Backend (`opengl_rhi.cpp`)
Conterá funções exclusivas de Kernel GDI para subida de Contexto de vídeo: `wglCreateContext`, `SwapBuffers` e `glClear`.

## Verification Plan
Lançaremos unit tests isolados no CTest onde tentaremos disparar calls de Render nulas. No Sandbox, invadiremos o Loop atráves da chamadas de RenderCommand, despachando um `ClearColor` explícito (Magenta/Azul) alterado pelo Hardware de Placa de Vídeo. Escala de Sucesso: Tela com Core preenchida indicando SwapBuffer.
