#include "test_core.h"
#include <canino/core/memory.h>
#include <canino/core/memory_arena.h>
#include <canino/core/memory_pool.h>

static bool Test_AlignForward() {
    uintptr_t base = 0x0001;
    // O hardware vai prever bit-a-bit se o shift é executado em frames contiguos estritos
    CANINO_EXPECT(canino::AlignForward(base, 8) == 0x0008);
    CANINO_EXPECT(canino::AlignForward(0x0008, 8) == 0x0008);
    CANINO_EXPECT(canino::AlignForward(0x0009, 16) == 0x0010);
    return true;
}

static bool Test_MemoryArena() {
    canino::MemoryArena arena;
    arena.InitializeHeap(1024);
    
    CANINO_EXPECT(arena.GetCapacity() == 1024);
    CANINO_EXPECT(arena.GetOffset() == 0);

    // Bumper
    void* p1 = arena.PushSize(16);
    CANINO_EXPECT(p1 != nullptr);
    CANINO_EXPECT(arena.GetOffset() == 16);

    // Padding de alinhamento forçado. Começamos a pedir tamanho 16 mas deslocado pra bitframe 32 limitante.
    // Ele estava em 16. Mover pro proximo alinhamento de 32 leva ele pro byte 32 vazio absoltuo. Logo +16(size) = 48 bytes comsumidos.
    void* p2 = arena.PushSize(16, 32); 
    CANINO_EXPECT(p2 != nullptr);
    CANINO_EXPECT(arena.GetOffset() == 48);

    // Clear maciço reseta o pointer (sem frees) O(1)
    arena.Clear();
    CANINO_EXPECT(arena.GetOffset() == 0);

    arena.DestroyHeap();
    return true;
}

static bool Test_MemoryPool() {
    canino::MemoryPool pool;
    pool.InitializeHeap(32, 10); // 10 blocos fixos na lista encadeada do hardware
    
    CANINO_EXPECT(pool.GetChunkSize() >= 32);
    
    void* p1 = pool.Allocate();
    void* p2 = pool.Allocate();
    
    CANINO_EXPECT(p1 != nullptr && p2 != nullptr);
    // Endereços contiguos da placa e independentes unicos
    CANINO_EXPECT(p1 != p2); 

    // Destrói no LIFO free-list
    pool.Free(p1);
    
    // Proxima carga DEVE usar em cache-hit absoluto o frame que acabou de secar
    void* p3 = pool.Allocate();
    CANINO_EXPECT(p3 == p1); 
    
    pool.DestroyHeap();
    return true;
}

bool RunMemoryTests() {
    CANINO_RUN_TEST(Test_AlignForward);
    CANINO_RUN_TEST(Test_MemoryArena);
    CANINO_RUN_TEST(Test_MemoryPool);
    return true;
}
