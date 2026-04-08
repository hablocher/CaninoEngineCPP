#pragma once
#include "core.h"
#include <cstdint>
#include <cstddef>

namespace canino {
    // Alinha um endereço de memória bitwise em um frame de N potências de dois (ex: 8, 16, 32 bytes).
    CANINO_API uintptr_t AlignForward(uintptr_t ptr, size_t alignment);
    CANINO_API void* AlignForward(void* ptr, size_t alignment);
}
