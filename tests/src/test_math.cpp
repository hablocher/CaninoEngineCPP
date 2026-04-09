#include "test_core.h"
#include <canino/math/math_ops.h>

static bool Test_Vec4_Aligned_Creation() {
    // Avalia o compilador forçando MSVC respeitar o alinhamento de cache de 128 bit
    canino::Vec4 target;
    CANINO_EXPECT((reinterpret_cast<uintptr_t>(&target) % 16) == 0);
    return true;
}

static bool Test_Intrinsics_Operations() {
    canino::Vec4 a = { 1.0f, 2.0f, 3.0f, 4.0f };
    canino::Vec4 b = { 10.0f, 20.0f, 30.0f, 40.0f };

    canino::Vec4 sum = canino::math::Add(a, b);
    CANINO_EXPECT(sum.x == 11.0f && sum.w == 44.0f);

    canino::Vec4 ext = canino::math::Sub(b, a);
    CANINO_EXPECT(ext.y == 18.0f && ext.z == 27.0f);

    canino::Vec4 mul = canino::math::Mul(a, b);
    CANINO_EXPECT(mul.x == 10.0f && mul.w == 160.0f);

    return true;
}

static bool Test_DotProduct() {
    canino::Vec4 a = { 1.0f, 2.0f, 3.0f, 4.0f };
    canino::Vec4 b = { 2.0f, 0.0f, 1.0f, -1.0f };
    // Dot = 1*2 + 2*0 + 3*1 + 4*-1 = 2 + 0 + 3 - 4 = 1

    float dot = canino::math::Dot(a, b);
    CANINO_EXPECT(dot == 1.0f);
    
    return true;
}

bool RunMathTests() {
    CANINO_RUN_TEST(Test_Vec4_Aligned_Creation);
    CANINO_RUN_TEST(Test_Intrinsics_Operations);
    CANINO_RUN_TEST(Test_DotProduct);
    return true;
}
