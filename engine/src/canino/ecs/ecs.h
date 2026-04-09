#pragma once
#include <canino/core/core.h>
#include <cstdint>

namespace canino {
    // Entities puras são apenas um inteiro O(1). Zero Virtual Arrays ou Strings.
    typedef uint32_t Entity;
    const Entity CANINO_NULL_ENTITY = 0xFFFFFFFF; // Entity limit max

    // Max entities permitidas puramente pra bater os pools alocados estáticamente pra velocidade 
    const uint32_t CANINO_MAX_ENTITIES = 8192;

    // CTE HASH - FNV-1a Hash Algorithm em Compile-Time Absoluto (C++11 constexpr)
    // Usado pra transformar "struct canino::Transform" em um Interiro 32-bits unívoco na Memoria.
    constexpr uint32_t CTE_HashString(const char* str) {
        uint32_t hash = 2166136261u;
        for (int i = 0; str[i]; ++i) {
            hash ^= (uint8_t)str[i];
            hash *= 16777619u;
        }
        return hash;
    }

    // Resolve as colisões inter-dll de C++.
    template<typename T>
    constexpr uint32_t GetComponentTypeId() {
#if defined(_MSC_VER)
        return CTE_HashString(__FUNCSIG__); // MSVC Signature
#else
        return CTE_HashString(__PRETTY_FUNCTION__); // Clang/GCC
#endif
    }
}
