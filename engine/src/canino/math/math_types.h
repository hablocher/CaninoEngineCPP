#pragma once
#include <canino/core/core.h>

// Header dependente de Intrinsicos SIMD Base (MSVC x64/Clang). Fornece __m128.
#include <immintrin.h>

namespace canino {
    // Escalares baseados em estruturas rasas O(1)
    struct Vec2 { float x, y; };
    struct Vec3 { float x, y, z; };

    // SSE 128-Bits Rigidamente Alocado p/ Otimizacao massiva Data-Oriented
    struct alignas(16) Vec4 {
        union {
            struct { float x, y, z, w; };
            __m128 simd; // O Segredo da brutalidade matemática x64
        };
    };

    // Matrizes para Render Hardware Interface (4x4 Colunas, 64-bytes alinhadas em L1 cache lines)
    struct alignas(16) Mat4 {
        union {
            float m[4][4];
            __m128 cols[4];
        };
    };
}
