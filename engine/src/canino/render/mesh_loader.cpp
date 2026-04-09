#include <canino/render/mesh_loader.h>
#include <iostream>

#pragma warning(disable: 4505)
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace canino {

    MeshDataRaw MeshLoader::LoadOBJ(const char* filepath) {
        MeshDataRaw rawData;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
            std::cout << "[MeshLoader] Fudeu. Arquivo " << filepath << " explodiu: " << warn << " " << err << std::endl;
            return rawData;
        }

        // Cache Unrolling
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                canino::RenderCommand::Vertex3D vertex = {};

                // Extrai as posições em Arrays Continuos (x, y, z)
                vertex.pos[0] = attrib.vertices[3 * index.vertex_index + 0];
                vertex.pos[1] = attrib.vertices[3 * index.vertex_index + 1];
                vertex.pos[2] = attrib.vertices[3 * index.vertex_index + 2];

                // Extrai UVs invertendo V (DirectX Style)
                if (index.texcoord_index >= 0) {
                    vertex.uv[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.uv[1] = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]; // Reverse V-Axis for D3D LeftHanded!
                } else {
                    vertex.uv[0] = 0.0f;
                    vertex.uv[1] = 0.0f;
                }

                rawData.Vertices.push_back(vertex);
                // Triangulando Flat na unha
                rawData.Indices.push_back((unsigned int)rawData.Indices.size());
            }
        }
        
        return rawData;
    }

}
