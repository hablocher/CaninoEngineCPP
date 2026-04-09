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
        res.m[0][0] = x; res.m[1][1] = y; res.m[2][2] = z;
        return res;
    }

    // ----------------------------------------------------
    // FPS VECTOR MATH (C-Style)
    // ----------------------------------------------------
    inline Vec3 Vec3Add(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    inline Vec3 Vec3Sub(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    inline Vec3 Vec3Mul(const Vec3& a, float s) { return {a.x * s, a.y * s, a.z * s}; }
    inline float Vec3Dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    
    inline Vec3 Vec3Cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    
    inline float Vec3Length(const Vec3& a) { return sqrtf(Vec3Dot(a, a)); }
    
    inline Vec3 Vec3Normalize(const Vec3& a) {
        float l = Vec3Length(a);
        if (l == 0.0f) return {0,0,0};
        return {a.x/l, a.y/l, a.z/l};
    }

    // Pipeline Quake/Doom FPS View LookAt Left-Handed
    inline Mat4 Mat4LookAt(const Vec3& eye, const Vec3& forward, const Vec3& up) {
        Vec3 zAxis = Vec3Normalize(forward);
        Vec3 xAxis = Vec3Normalize(Vec3Cross(up, zAxis));
        Vec3 yAxis = Vec3Cross(zAxis, xAxis);

        Mat4 res = Mat4Identity();
        // Matrix View Reversa pra Rotacionar o Cenário contra o Jogador (Row-Major memory layout)
        res.m[0][0] = xAxis.x; res.m[1][0] = xAxis.y; res.m[2][0] = xAxis.z; res.m[3][0] = -Vec3Dot(xAxis, eye);
        res.m[0][1] = yAxis.x; res.m[1][1] = yAxis.y; res.m[2][1] = yAxis.z; res.m[3][1] = -Vec3Dot(yAxis, eye);
        res.m[0][2] = zAxis.x; res.m[1][2] = zAxis.y; res.m[2][2] = zAxis.z; res.m[3][2] = -Vec3Dot(zAxis, eye);
        return res;
    }

}
}
