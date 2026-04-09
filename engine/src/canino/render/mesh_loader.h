#pragma once
#include <canino/core/core.h>
#include <canino/render/rhi.h>
#include <vector>

namespace canino {
    struct MeshDataRaw {
        std::vector<canino::RenderCommand::Vertex3D> Vertices;
        std::vector<unsigned int> Indices;
    };

    class CANINO_API MeshLoader {
    public:
        // Parseia assincronamente a estrutura bruta do sistema para Injesão na VRAM
        static MeshDataRaw LoadOBJ(const char* filepath);
    };
}
