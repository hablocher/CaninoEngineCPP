#include "memory.h"
#include <cassert>

namespace canino {
    uintptr_t AlignForward(uintptr_t ptr, size_t alignment) {
        // Alinhamento deve obrigatoriamente ser potência de 2 no hardware
        CANINO_ASSERT((alignment & (alignment - 1)) == 0);

        uintptr_t p = ptr;
        uintptr_t a = (uintptr_t)alignment;
        
        // Empurra o ponteiro até o limite que encaixa
        uintptr_t modulo = p & (a - 1);
        if (modulo != 0) {
            p += a - modulo;
        }
        return p;
    }

    void* AlignForward(void* ptr, size_t alignment) {
        uintptr_t aligned = AlignForward((uintptr_t)ptr, alignment);
        return (void*)aligned;
    }
}
