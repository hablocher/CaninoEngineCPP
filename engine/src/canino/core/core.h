#pragma once

#ifdef _WIN32
    #ifdef CANINO_BUILD_DLL
        #define CANINO_API __declspec(dllexport)
    #else
        #define CANINO_API __declspec(dllimport)
    #endif
#else
    #define CANINO_API __attribute__((visibility("default")))
#endif

// Assertion básica
#include <cassert>
#define CANINO_ASSERT(x, ...) assert(x)

namespace canino {
    // Teste inicial do Linker
    CANINO_API void Initialize();
}
