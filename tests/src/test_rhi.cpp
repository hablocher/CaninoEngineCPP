#include "test_core.h"
#include <canino/math/math_types.h>

// Mock das Memórias de Device Context que usamos no backend DX11
// Como a memória de layout C++ e VRAM do D3D11 exige empacotamento exato,
// criamos uma struct paralela testável para proteger o alinhamento.
struct alignas(16) CBOMatrices {
    canino::Mat4 MVP;
};

struct alignas(16) CBOMaterials {
    int UseTexture[4];
    float SolidColor[4];
};

bool StructAlignmentDX11() {
    size_t sizeMatrices = sizeof(CBOMatrices);
    size_t sizeMaterials = sizeof(CBOMaterials);

    CANINO_EXPECT(sizeMatrices == 64);
    CANINO_EXPECT(sizeMaterials == 32);

    CANINO_EXPECT(sizeMatrices % 16 == 0);
    CANINO_EXPECT(sizeMaterials % 16 == 0);

    return true;
}

bool RunRHITests() {
    CANINO_RUN_TEST(StructAlignmentDX11);
    return true;
}
