#pragma once
#include <canino/ecs/ecs.h>
#include <unordered_map>
#include <vector>

namespace canino {

// Objeto base cego q gerencia remoções virtuais invisível para tipagem forte do Engine registry
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
private:
    // O Milagre DOD - Os dados perfeitos ensanduichados para Itareções de Cache-Lines puras O(1)
    T m_ComponentArray[CANINO_MAX_ENTITIES];
    
    // Arrays Mapeadoras pra destruição do Swap and Pop
    Entity m_IndexToEntity[CANINO_MAX_ENTITIES]; 
    Entity m_EntityToIndex[CANINO_MAX_ENTITIES]; 
    
    uint32_t m_Size;

public:
    ComponentArray() {
        m_Size = 0;
        for(uint32_t i=0; i<CANINO_MAX_ENTITIES; ++i) {
            m_EntityToIndex[i] = CANINO_NULL_ENTITY;
            m_IndexToEntity[i] = CANINO_NULL_ENTITY;
        }
    }

    void InsertData(Entity entity, T component) {
        // Mapeia e injeta o componente sempre na calda (Totalmente denso sem vacuos pra performance Maxima)
        uint32_t newIndex = m_Size;
        m_EntityToIndex[entity] = newIndex;
        m_IndexToEntity[newIndex] = entity;
        m_ComponentArray[newIndex] = component;
        ++m_Size;
    }

    void RemoveData(Entity entity) {
        // Checando se a Array Oca contem a referencia dessa arvore morta
        uint32_t indexOfRemovedEntity = m_EntityToIndex[entity];
        if (indexOfRemovedEntity == CANINO_NULL_ENTITY) return;

        // The DOD SWAP AND POP Trick
        // Mover o ULTIMO elemento do array pra cobrir o buraco, evitando Memory Shift em Ram pesada
        uint32_t indexOfLastElement = m_Size - 1;
        Entity entityOfLastElement = m_IndexToEntity[indexOfLastElement];

        // Mover a Memoria Crua do Fundo pro Buraco
        m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

        // Atualizar Tabela Mestra (Index Maps Oca)
        m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;

        // Inutiliza os restos lixosos do falecido
        m_EntityToIndex[entity] = CANINO_NULL_ENTITY;
        m_IndexToEntity[indexOfLastElement] = CANINO_NULL_ENTITY;

        --m_Size;
    }

    T* GetData(Entity entity) {
        uint32_t index = m_EntityToIndex[entity];
        if (index == CANINO_NULL_ENTITY) return nullptr;
        return &m_ComponentArray[index];
    }

    void EntityDestroyed(Entity entity) override {
        // Evento virtual disparado indiretamente por Registry Master-Control
        RemoveData(entity);
    }
};

} // namespace
