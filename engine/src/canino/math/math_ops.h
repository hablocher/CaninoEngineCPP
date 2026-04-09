#pragma once
#include <canino/math/math_types.h>

namespace canino {
namespace math {

    // ----------------------------------------------------
    // INTRINSIC VECTOR PIPELINES (Cycle-Perfect Additions)
    // ----------------------------------------------------
    
    inline Vec4 Add(const Vec4& a, const Vec4& b) {
        Vec4 out;
        // 1 unico ciclo mandando VADDPS (Add Packed Single-Precision)
        out.simd = _mm_add_ps(a.simd, b.simd);
        return out;
    }

    inline Vec4 Sub(const Vec4& a, const Vec4& b) {
        Vec4 out;
        out.simd = _mm_sub_ps(a.simd, b.simd);
        return out;
    }

    inline Vec4 Mul(const Vec4& a, const Vec4& b) {
        Vec4 out;
        out.simd = _mm_mul_ps(a.simd, b.simd);
        return out;
    }

    // Calcula DotProduct purista pra SIMD antigo + moderno via Shuffles manuais brutais
    inline float Dot(const Vec4& a, const Vec4& b) {
        __m128 mul = _mm_mul_ps(a.simd, b.simd); // X*X, Y*Y, Z*Z, W*W simultaneos num clock
        
        // Magia Negra SSE: Movemos e embaralhamos as somas para dentro até sobrar num único escalar (1º indice)
        __m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 sums = _mm_add_ps(mul, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        
        // Dispara de volta do Cópula para Memoria (Converter Scalar System)
        return _mm_cvtss_f32(sums);
    }

}
}
