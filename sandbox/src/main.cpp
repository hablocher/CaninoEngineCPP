#include <canino/core/core.h>
#include <canino/core/memory_arena.h>
#include <iostream>
#include <chrono>

struct Transform {
    float position[3];
    float rotation[4];
    float scale[3];
};

int main() {
    std::cout << "[Sandbox] Iniciando o Benchmark de Memoria DOD..." << std::endl;
    // canino::Initialize(); // ignorando a inicializacao default pro benchmark

    const size_t ALLOCATION_COUNT = 1000000; // 1 milhao de alocacoes
    
    // --- Teste 1: Padrão C++ 'new/delete' ---
    {
        std::cout << "\n--- Run: Standard OS Heap (new/delete) ---" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        Transform** transforms = new Transform*[ALLOCATION_COUNT];
        for (size_t i = 0; i < ALLOCATION_COUNT; ++i) {
            transforms[i] = new Transform();
            transforms[i]->position[0] = 1.0f; // workload fantasma
        }
        
        // Simulação bruta de destruição temporal
        for (size_t i = 0; i < ALLOCATION_COUNT; ++i) {
            delete transforms[i];
        }
        delete[] transforms;

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Tempo HEAP: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
                  << " ms" << std::endl;
    }

    // --- Teste 2: Memory Arena Data-Oriented ---
    {
        std::cout << "\n--- Run: Canino Linear Memory Arena ---" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        canino::MemoryArena arena;
        arena.InitializeHeap(sizeof(Transform) * ALLOCATION_COUNT + 1024); // Somente 1 System Call O.S.

        for (size_t i = 0; i < ALLOCATION_COUNT; ++i) {
            // Empurra pointer O(1) de forma alinhada
            Transform* t = canino::PushStruct<Transform>(arena);
            t->position[0] = 1.0f; // workload
        }
        
        // Limpeza maciça O(1)
        arena.Clear();
        arena.DestroyHeap();

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Tempo ARENA: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
                  << " ms" << std::endl;
    }

    std::cout << "\nBenchmark concluido com exito." << std::endl;
    return 0;
}
