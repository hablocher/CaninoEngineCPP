# Design: The Math Core (SIMD & Data-Oriented)

Cálculos tridimensionais exigem poder bruto. Criaremos o canino/math para usar Registradores XMM via Intrínsecos C.

## Resoluções de Design (Aprovadas)
1. **The Alignment Rule**: Faremos uso do restrito comando `alignas(16)`. Arrays em DOD de translação (Vec4, Mat4) obedecerão layouts de 128-bits exatos sem tolerâncias de padding OS, prevenindo Cache Misses absolutos nas *Arenas*.

## Estrutura The Math Core
1. Cabeçalho Central `canino/math/math_types.h`: Modelagem canônica Data-Oriented unificando Structs anônimas a Memórias raw de Assembly (ex: `__m128 simd;`).
2. API Purista e Inline em `canino/math/math_ops.h`: Para que a CPU evite *Function Pointers*, `Math_Add()` e `Math_Dot()` agirão como extensores textuais de hardware durante a fase de linkagem do MSVC, chamando micro-códigos puros da SSE de forma transparente.

## Verification Plan
Adicionaremos `test_math.cpp` na Suite Purista. Exigiremos com assertions matemáticas que Cálculos escalares lentos resultem exatamente na mesmíssima coisa que um `_mm_add_ps` vetorial. Executaremos através do CTest local e conferiremos 100% test pass!
