# Fundação do Build System: CMake Avançado

A fundação do nosso *Build System* está completa e validada ponta-a-ponta em ambiente Windows (MSVC 19.44). 

## O que foi implementado

*   **Arquitetura Target-Based Centralizada:** Criamos o `CMakeLists.txt` raiz para coordenar globalmente o projeto estipulando C++20 de forma violenta (`CMAKE_CXX_EXTENSIONS OFF`).
*   **Controle de Output Unificado:** Exigimos que todos os artefatos de tempo de compilação ou execução(`.exe`, `.dll`, `.lib`) terminassem no diretório global `build/bin/` ou `build/lib/`. Isso alivia pesadelos de diretórios de trabalho em engines dinâmicas.
*   **Engine DLL Boundary (`core.h`):** O target `CaninoEngine` foi mapeado para compilador como uma bibliotéca dinâmica (`SHARED`). Adicionamos as proteções `CANINO_API` utilizando a macro `__declspec(dllexport/dllimport)` vitalícias para o limite da engine.
*   **Target Sandbox:** Criamos um cliente de teste que efetua *linking* privado com a `CaninoEngine` de forma indolor usando targets e public interfaces nativas do CMake.

## Resultados da Validação

A compilação local pelo MSVC obteve êxito absoluto (*Exit Code: 0*) usando C++20 Standard.
Ao rodar a aplicação base, obtivemos o *handshake* satisfatório entre os binários:

```shell
[Sandbox] App Starting up...
[Canino Engine] Core Initialize called.
```

> [!TIP]
> **Data-Oriented Thinking**: O fato de termos *Warnings as Errors* (com exceção de *nameless struct/unions* para SIMD) garante que *layouts* incorretos de struct não passarão pra dentro da Base de código sem notarmos via alertas de alinhamento e _padding_ mortos.

Estamos prontos para prosseguir com as fundações críticas de memória da Engine.
