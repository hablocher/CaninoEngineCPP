#include "memory_arena.h"
#include "memory.h"
#include <cstdlib>

namespace canino {
    MemoryArena::MemoryArena() : m_Buffer(nullptr), m_Capacity(0), m_Offset(0) {}

    MemoryArena::~MemoryArena() {}

    void MemoryArena::InitializeHeap(size_t sizeBytes) {
        CANINO_ASSERT(m_Buffer == nullptr);
        // Um e apenas UM acesso pesado ao SO.
        m_Buffer = (unsigned char*)std::malloc(sizeBytes);
        m_Capacity = sizeBytes;
        m_Offset = 0;
    }

    void MemoryArena::DestroyHeap() {
        if (m_Buffer) {
            std::free(m_Buffer);
            m_Buffer = nullptr;
            m_Capacity = 0;
            m_Offset = 0;
        }
    }

    void* MemoryArena::PushSize(size_t size, size_t alignment) {
        uintptr_t currentPtr = (uintptr_t)m_Buffer + (uintptr_t)m_Offset;
        uintptr_t offsetAbs = AlignForward(currentPtr, alignment);
        
        // Retorna pro espaço relativo (offset bytes from base)
        uintptr_t offsetSafe = offsetAbs - (uintptr_t)m_Buffer; 
        
        CANINO_ASSERT(offsetSafe + size <= m_Capacity && "[MemoryArena] Out of Memory! Aumente o backing allocator.");
        if (offsetSafe + size > m_Capacity) {
            return nullptr;
        }

        void* ptr = &m_Buffer[offsetSafe];
        m_Offset = offsetSafe + size;
        return ptr;
    }

    void MemoryArena::Clear() {
        m_Offset = 0;
    }
}
