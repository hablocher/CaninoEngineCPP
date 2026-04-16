#pragma once
#include <canino/core/core.h>
#include <canino/math/math_ops.h>

namespace canino {
    
    // Axis-Aligned Bounding Box (Caixa de Colisão que nunca rotaciona - padrão Quake/Source)
    struct AABB {
        Vec3 min;
        Vec3 max;
    };

    struct CollisionHit {
        bool hasHit;
        Vec3 penetration; // O vetor MTV (Minimum Translation Vector) para tirar a malha presa na parede
    };

    namespace physics {
        
        // Inicializa o Engine Físico (Pós-alocação e preenchimento de Grids futuros)
        CANINO_API void Init(); 

        // Resolve colisão entre um objeto dinâmico (Player) e um estático (Parede) em 3D Absoluto
        CANINO_API CollisionHit ResolveAABB(const AABB& dynamicBox, const AABB& staticBox);
        
        // Construtor utilitário para derivar um cubo físico de um ponto X,Y,Z com escala Width,Height,Depth
        inline AABB AABBFromTransform(const Vec3& pos, const Vec3& scale) {
            return {
                { pos.x - scale.x * 0.5f, pos.y - scale.y * 0.5f, pos.z - scale.z * 0.5f },
                { pos.x + scale.x * 0.5f, pos.y + scale.y * 0.5f, pos.z + scale.z * 0.5f }
            };
        }
    }
}
