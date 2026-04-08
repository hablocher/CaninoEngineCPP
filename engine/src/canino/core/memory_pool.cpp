#include "memory_pool.h"
#include "memory.h"
#include <cstdlib>

namespace canino {
    MemoryPool::MemoryPool() : m_Buffer(nullptr), m_Capacity(0), m_ChunkSize(0), m_FreeListHead(nullptr) {}

    MemoryPool::~MemoryPool() {}

    void MemoryPool::InitializeHeap(size_t chunkSize, size_t chunkCount) {
        CANINO_ASSERT(m_Buffer == nullptr);
        
        // Nó do FreeNode tem que caber no chunk (em 64 bits = 8 bytes de ponteiro minimo)
        m_ChunkSize = chunkSize < sizeof(FreeNode) ? sizeof(FreeNode) : chunkSize;
        
        // Aplicamos padrão forte e indestrutível de 8 bytes p/ structs
        m_ChunkSize = (m_ChunkSize + 7) & ~7; 

        m_Capacity = m_ChunkSize * chunkCount;
        m_Buffer = (unsigned char*)std::malloc(m_Capacity);

        // Gera e 'espreme' um linked-list listado através dos frames da memória física
        m_FreeListHead = (FreeNode*)m_Buffer;
        FreeNode* current = m_FreeListHead;
        for (size_t i = 1; i < chunkCount; ++i) {
            FreeNode* nextNode = (FreeNode*)(m_Buffer + (i * m_ChunkSize));
            current->next = nextNode;
            current = nextNode;
        }
        current->next = nullptr;
    }

    void MemoryPool::DestroyHeap() {
        if (m_Buffer) {
            std::free(m_Buffer);
            m_Buffer = nullptr;
            m_Capacity = 0;
            m_ChunkSize = 0;
            m_FreeListHead = nullptr;
        }
    }

    void* MemoryPool::Allocate() {
        if (m_FreeListHead == nullptr) {
            CANINO_ASSERT(false && "[MemoryPool] Esgotado! Instâncias demais geradas.");
            return nullptr;
        }

        // Tira o nodo do top list (O(1)) e ele mesmo se torna payload
        FreeNode* node = m_FreeListHead;
        m_FreeListHead = m_FreeListHead->next;
        return (void*)node;
    }

    void MemoryPool::Free(void* ptr) {
        if (ptr == nullptr) return;

        // Ao matar uma entity/pointer, transformamos aquele frame em um pointer pra lista
        FreeNode* node = (FreeNode*)ptr;
        node->next = m_FreeListHead;
        m_FreeListHead = node;
    }
}
