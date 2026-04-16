#pragma once
#include <iostream>

// Syntax-sugar pre-processado. Extrema velocidade e footprint de memoria 0 na engine core.
// Interceptamos as macros para que as falhas digam arquivo e linha exatos no hardware

#define CANINO_RUN_TEST(func) \
    do { \
        std::cout << "[TEST] Rodando \033[36m" << #func << "\033[0m..." << std::endl; \
        if (!func()) { \
            std::cerr << "\033[31m[FALHA FATAL]\033[0m Integridade comprometida em " << #func << std::endl; \
            return false; \
        } \
    } while(false)

#define CANINO_EXPECT(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "  -> \033[31mASSERT FALHOU:\033[0m " << #cond << " no Arquivo " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(false)

// Prototipos das rotinas do DOD Test
bool RunMemoryTests();
bool RunPlatformTests();
bool RunInputTests();
bool RunMathTests();
bool RunThreadingTests();
bool RunECSTests();
bool RunRHITests();
