#pragma once
#include <canino/math/math_types.h>
#include <math.h>

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

    // Mutações matriciais nulas
    inline Mat4 Mat4Identity() {
        Mat4 res = {0};
        res.m[0][0] = 1.0f; res.m[1][1] = 1.0f;
        res.m[2][2] = 1.0f; res.m[3][3] = 1.0f;
        return res;
    }

    // Pipeline Algebra linear de Rotação Eixo X
    inline Mat4 Mat4RotateX(float angleRad) {
        Mat4 res = Mat4Identity();
        float c = cosf(angleRad);
        float s = sinf(angleRad);
        res.m[1][1] = c;  res.m[1][2] = s;
        res.m[2][1] = -s; res.m[2][2] = c;
        return res;
    }

    // Pipeline Algebra linear de Rotação Eixo Y
    inline Mat4 Mat4RotateY(float angleRad) {
        Mat4 res = Mat4Identity();
        float c = cosf(angleRad);
        float s = sinf(angleRad);
        res.m[0][0] = c;  res.m[0][2] = -s;
        res.m[2][0] = s;  res.m[2][2] = c;
        return res;
    }

    // Multiplicação Massiva Matricial CPU
    inline Mat4 Mat4Multiply(const Mat4& a, const Mat4& b) {
        Mat4 res = {0};
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                res.m[i][j] = a.m[i][0] * b.m[0][j] +
                              a.m[i][1] * b.m[1][j] +
                              a.m[i][2] * b.m[2][j] +
                              a.m[i][3] * b.m[3][j];
            }
        }
        return res;
    }

    // Projeção Frustum (Converte Visão Lente pra NDC Shader)
    inline Mat4 Mat4Perspective(float fovRad, float aspect, float nearZ, float farZ) {
        Mat4 res = {0};
        float f = 1.0f / tanf(fovRad / 2.0f);
        res.m[0][0] = f / aspect;
        res.m[1][1] = f;
        res.m[2][2] = farZ / (farZ - nearZ);
        res.m[2][3] = 1.0f; // Salva W pra divisao perspetiva pela Placa
        res.m[3][2] = (-(nearZ * farZ)) / (farZ - nearZ);
        return res;
    }

    // Translação
    inline Mat4 Mat4Translate(float x, float y, float z) {
        Mat4 res = Mat4Identity();
        res.m[3][0] = x;
        res.m[3][1] = y;
        res.m[3][2] = z;
        return res;
    }

    // Escala
    inline Mat4 Mat4Scale(float x, float y, float z) {
        Mat4 res = Mat4Identity();
        res.m[0][0] = x;
        res.m[1][1] = y;
        res.m[2][2] = z;
        return res;
    }

}
}
