#pragma once
#include <canino/core/core.h>

namespace canino {
    // Parser purista C-Style focado em O(1) Lookups Lineares sobre Cache-File. Null Allocations (Heap).
    class ConfigParser {
    public:
        CANINO_API static void LoadFromFile(const char* filepath);
        CANINO_API static bool GetString(const char* key, char* outBuffer, unsigned int bufferSize);
    };
}
