#include <canino/ecs/registry.h>

namespace canino {

Registry::Registry() {
    m_LivingEntityCount = 0;
    // Puxa toda array de mortos com ids livres de imediato no boot para nao realocarmos no frame
    for (Entity i = 0; i < CANINO_MAX_ENTITIES; ++i) {
        m_AvailableEntities.push(i);
    }
}

Entity Registry::CreateEntity() {
    if (m_LivingEntityCount >= CANINO_MAX_ENTITIES) {
        // Assert violento Data-oriented. Extrapolamos os componentes de VRAM/RAM 
        return CANINO_NULL_ENTITY;
    }
    
    Entity id = m_AvailableEntities.front();
    m_AvailableEntities.pop();
    ++m_LivingEntityCount;
    return id;
}

void Registry::DestroyEntity(Entity entity) {
    if (entity >= CANINO_MAX_ENTITIES) return;

    // Tira os intestinos do C-pointer em todos os blocos logicos DOD pra reuso de lixo
    for (auto const& pair : m_ComponentArrays) {
        pair.second->EntityDestroyed(entity);
    }

    // Retorna a licenca do ID pra matar de novo
    m_AvailableEntities.push(entity);
    --m_LivingEntityCount;
}

}
