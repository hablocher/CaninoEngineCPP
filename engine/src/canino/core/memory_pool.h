#pragma once
#include "core.h"
#include <cstddef>

namespace canino {
    // Alocador de chunk fixo por FreeList
    class CANINO_API MemoryPool {
    private:
        struct FreeNode {
            FreeNode* next;
        };

        unsigned char* m_Buffer;
        size_t m_Capacity;
        size_t m_ChunkSize;
        FreeNode* m_FreeListHead;

    public:
        MemoryPool();
        ~MemoryPool();

        void InitializeHeap(size_t chunkSize, size_t chunkCount);
        void DestroyHeap();

        void* Allocate();
        void  Free(void* ptr);

        inline size_t GetChunkSize() const { return m_ChunkSize; }
    };
}
