#include "test_core.h"
#include <canino/core/core.h>
#include <iostream>

int main() {
    std::cout << "\n============================================\n";
    std::cout << "--- CANINO ENGINE PURIST TEST SUITE ---\n";
    std::cout << "============================================\n" << std::endl;

    bool success = true;
    
    if (!RunMemoryTests()) success = false;
    if (!RunPlatformTests()) success = false;
    if (!RunInputTests()) success = false;

    if (success) {
        std::cout << "\n\033[32m[PASSOU]\033[0m Todos os sistemas e endereços da Canino Engine testados com sucesso absoluto." << std::endl;
        return 0; // Sinaliza Test runner OS CTest que estavel
    } else {
        std::cerr << "\n\033[31m[FALHA NO BUILD]\033[0m Falhas extremas de Kernel/Memoria detectadas. Abortando deploy." << std::endl;
        return 1; // Dispara erro Pipeline CMake
    }
}
