#pragma once
#include "core.h"
#include <cstddef>

namespace canino {
    // Alocador O(1), estrito em linearidade e sem fragm. 
    class CANINO_API MemoryArena {
    private:
        unsigned char* m_Buffer;
        size_t m_Capacity;
        size_t m_Offset;

    public:
        MemoryArena();
        ~MemoryArena();

        // Backend allocation: Usa o Heap Global só uma vez no setup
        void InitializeHeap(size_t sizeBytes);
        void DestroyHeap();

        // Puxa tamanho da arena e ajusta ponteiro. Nunca chama `new`.
        void* PushSize(size_t size, size_t alignment = 8);
        
        // Instantaneamente 'deleta' todos objetos alocados aqui abaixando offset para 0.
        void Clear();

        inline size_t GetOffset() const { return m_Offset; }
        inline size_t GetCapacity() const { return m_Capacity; }
    };

    // Helper syntax-sugar via templates (Totalmente INLINED, sem jump symbol overhead)
    template<typename T>
    inline T* PushStruct(MemoryArena& arena, size_t count = 1, size_t alignment = alignof(T)) {
        return static_cast<T*>(arena.PushSize(sizeof(T) * count, alignment));
    }
}
