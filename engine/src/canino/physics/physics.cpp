#include <canino/physics/physics.h>
#include <math.h>

namespace canino {
namespace physics {

    void Init() {
        // Reservado para Multi-threading e Spatial Partitioning nas próximas updates
    }

    CollisionHit ResolveAABB(const AABB& dyna, const AABB& stat) {
        CollisionHit hit = { false, {0.0f, 0.0f, 0.0f} };

        // Teste O(1) puro do Sweeping - Quais frentes atravessaram quais paredes
        float dx1 = stat.max.x - dyna.min.x; // Boneco esmagado pra Direita (Parede esquerda contra a direita do player não acontece) -> Penetração da direita na parede? 
        float dx2 = stat.min.x - dyna.max.x; // Boneco batendo numa parede que está na sua direita
        float dy1 = stat.max.y - dyna.min.y; // Chão/Teto
        float dy2 = stat.min.y - dyna.max.y; 
        float dz1 = stat.max.z - dyna.min.z; // Fundo/Frente
        float dz2 = stat.min.z - dyna.max.z;

        // Regra clássica do AABB: Se qualquer face de 'A' está completamente fora de QUALQUER face de 'B', eles NÃO estão colidindo!
        // Ou seja, se o dx1 (parede direita - nosso pé esquerdo) for < 0, a parede sequer chegou na gente.
        if (dx1 <= 0.0f || dx2 >= 0.0f || dy1 <= 0.0f || dy2 >= 0.0f || dz1 <= 0.0f || dz2 >= 0.0f) {
            return hit; // Sai antecipadamente (Sem colisão)
        }

        hit.hasHit = true;

        // Encontramos quem entrou menos na caixa (pois o menor desvio é o correto de push-out fisicamente viável)
        float penX = (fabsf(dx1) < fabsf(dx2)) ? dx1 : dx2;
        float penY = (fabsf(dy1) < fabsf(dy2)) ? dy1 : dy2;
        float penZ = (fabsf(dz1) < fabsf(dz2)) ? dz1 : dz2;

        float absX = fabsf(penX);
        float absY = fabsf(penY);
        float absZ = fabsf(penZ);

        // Separação em um EIXO ÚNICO baseada no MTV (Minimum Translation Vector)
        if (absX < absY && absX < absZ) {
            hit.penetration.x = penX;
        } else if (absY < absX && absY < absZ) {
            hit.penetration.y = penY;
        } else {
            hit.penetration.z = penZ;
        }

        return hit;
    }

}
}
