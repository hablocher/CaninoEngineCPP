# Design: Platform Layer (Native Window)

Para mantermos a promessa de uma engine enxuta e fúriosa, estamos construindo a Engine de Janela usando diretamente a **Win32 API** no Windows (e estipulado arquiteturalmente `X11` / `Wayland` pra depois). Rejeitamos bibliotecas de middleware engordadas.

## Resoluções de Design (Aprovadas)

1. **Opaque C-Pointer Handler:** Usaremos Data-Oriented struct isolation de C puro. `engine/canino/platform/window.h` fará _forward declaration_ estrutural de `struct Window`, protegendo pra sempre toda a lógica visual, input e UI da contaminação das violentas macros de sistema (ex: `.h` do c-system do windows). O cabeçalho Win32 se limitará a `.cpp`.
2. **Escopo Contido (No Input agora):** Concentraremos todo o framework de Message Callback Loop da plataforma primária em subir e destruir uma janela cinza robusta, antes de inflar o parser pra ler Mouse Deltas ou ScanCodes de Keyboards.

## Topologia Proposta

Expandiremos a camada `canino/platform`.
- `canino/platform/window.h`: Opaque Handles p/ cliente
- `canino/platform/windows/window_win32.cpp`: Boilerplate massivo `#ifdef _WIN32` para erguer a Janela WinAPI e tratar Loop de mensagens.

## Verification Plan
Limparemos o main da aplicação e o direcionaremos estritamente pra bater e compilar o arquivo de Interface. Um loop gigante deve bombear eventos OS (como Alt+F4) de forma purista sem crashes em memory leaks.
