# Fundação da Platform Layer: OS Window System (Cru)

A primeira etapa visual da nossa engine está concluída. Removemos inteiramente o peso massivo de frameworks como o SDL/GLFW em prol do purismo da Kernel API.

## O que foi implementado

1.  **Strict Data-Oriented Opaque API (`window.h`)**: Construímos a barreira de isolamento primária. Pela primeira vez no projeto implementamos uma estrura não-visível (`struct Window;`) no header público. Isso erradica com 100% de precisão o vazamento das macros poluidoras do Windows (como `#define CreateWindow CreateWindowW`) pro resto do escopo da Engine.
2.  **OS Native Handlers (`window_win32.cpp`)**: A implementação exclusiva que só participa da Build Tree se `WIN32` for o sistema host. Construímos a classe visceral GDI, fizemos o hook de `WindowProc` e amarramos nossa struct polimorficamente limpa no `GWLP_USERDATA` do Hardware da Janela.
3.  **Real-Time Polling Loop**: Foi estruturado o *Event Pump* `PlatformPumpMessages()`, que puxa as requisições de sistema (`PeekMessage`) _sem interromper_ ou travar a CPU para o futuro render frame (Diferente do mortal `GetMessage` convencional que adormece threads).

## Resultados Empíricos (Validação Visual)

A nossa aplicação de testes de fundação (`SandboxApp`) foi alterada para engolir as inicializações C-Style:

```shell
[Sandbox] Iniciando Engine DOD e forjando Kernel Hardware Window...
[Sandbox] Instancia renderizada. Ingressando no Real-time Infinite Loop O(1).
```

Uma imponente estrutura GUI nativa do sistema operacional (renderizada através da placa mãe GDI temporariamente) agora existe de forma independente.  Pressionamentos vitais de interrupção forçada (`Alt+F4` e Clique no botão Fechar) deflagram com exatidão a desinstanciação de memória (CATCH -> Free -> DestroyWindow) provando a robustez zero-leaks da nossa base da *Canino*.
