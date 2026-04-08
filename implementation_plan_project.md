# Arquitetura do Build System (CMakeLists.txt)

Para a Canino Engine, estabeleceremos um build system limpo, moderno (target-based) e extremamente estrito. A estrutura será dividida entre a própria biblioteca/engine (`Canino`) e a aplicação cliente, como um `Sandbox` ou `Game` que consumirá a engine e nos servirá de ambiente de testes.

## Resoluções de Design (Aprovadas)

1. **Diretrizes SIMD**: Mantido genérico no nível macro do projeto. Detecção e intrinsics serão providenciados pelo pacote Core posteriormente.
2. **Tipo de Biblioteca**: Compilada como **Shared Library (DLL/SO)**. Isso exigirá macros de import/export (`__declspec(dllexport/dllimport)`) estritas na fronteira da Engine.

## Estrutura de Diretórios Proposta

Vamos criar a seguinte estrutura na pasta do repositório (`e:\Code\CPP\CaninoEngine`):

```text
CaninoEngine/
├── CMakeLists.txt              # Root CMakeLists
├── engine/                     # Código principal da Engine (compilado como biblioteca DLL)
│   ├── CMakeLists.txt          
│   └── src/
│       └── canino/             # Namespace canino base para organização
│           ├── core/           # Memory, Math, Job System (incluindo macros de export DLL)
│           ├── platform/       # Window, Input, file IO
│           └── render/         # RHI, RenderGraph
└── sandbox/                    # Aplicação cliente (Game Executable) para testar a engine
    ├── CMakeLists.txt          
    └── src/
        └── main.cpp
```

## Propriedades do Build

O CMake fará as seguintes configurações críticas globais:
1. **C++ Standard**: C++20 obrigatório (`CMAKE_CXX_STANDARD_REQUIRED ON`). Extensões do compilador desativadas (`CMAKE_CXX_EXTENSIONS OFF`) para manter portabilidade pura entre MSVC e Clang/GCC.
2. **Global Output Directories**: Direcionar todos os `.exe`, `.dll` e `.lib` construídos para uma pasta única de output (ex: `build/bin/` e `build/lib/`), facilitando a execução e o hot-reloading (futuro).
3. **Compiler Warnings as Errors**: Tratamento rigoroso de warnings.
    - **MSVC (Windows)**: `/W4 /WX /Zc:preprocessor`. Desativaremos temporariamente o `/wd4201` apenas para permitir *nameless struct/unions* em matemáticas SIMD.
    - **Clang/GCC (Linux)**: `-Wall -Wextra -Wpedantic -Werror`. Desativaremos o `-Wno-gnu-anonymous-struct`.

## Proposed Changes

Vou inicializar os arquivos `CMakeLists.txt` primários, incluindo um header básico de exportação para suportar o formato DLL.

#### [NEW] [CMakeLists.txt](file:///e:/Code/CPP/CaninoEngine/CMakeLists.txt)
O arquivo raiz.

#### [NEW] [engine/CMakeLists.txt](file:///e:/Code/CPP/CaninoEngine/engine/CMakeLists.txt)
Target `SHARED` CaninoEngine.

#### [NEW] [engine/src/canino/core/core.h](file:///e:/Code/CPP/CaninoEngine/engine/src/canino/core/core.h)
Definição da macro `CANINO_API` baseada na target de compilação.

#### [NEW] [sandbox/CMakeLists.txt](file:///e:/Code/CPP/CaninoEngine/sandbox/CMakeLists.txt)
Target executável `SandboxApp`.

#### [NEW] [sandbox/src/main.cpp](file:///e:/Code/CPP/CaninoEngine/sandbox/src/main.cpp)
Arquivo simples com o int main() para teste de build.

## Verification Plan

Vou rodar `cmake -B build` em sua máquina Windows para testar a geração. Se tudo correr bem, teremos os targets prontos.
