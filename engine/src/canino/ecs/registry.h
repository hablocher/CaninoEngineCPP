#pragma once
#include <canino/ecs/component_array.h>
#include <queue>
#include <memory>
#include <unordered_map>

namespace canino {

class Registry {
private:
    std::queue<Entity> m_AvailableEntities;
    uint32_t m_LivingEntityCount;

    // Hashes são alocados via map por conveniencia do CMake DLL sem arrays malucas com MAX_COMPONENTS
    std::unordered_map<uint32_t, std::shared_ptr<IComponentArray>> m_ComponentArrays;

    template<typename T>
    ComponentArray<T>* GetComponentArray() {
        uint32_t id = GetComponentTypeId<T>();
        if (m_ComponentArrays.find(id) == m_ComponentArrays.end()) {
            m_ComponentArrays[id] = std::make_shared<ComponentArray<T>>();
        }
        return static_cast<ComponentArray<T>*>(m_ComponentArrays[id].get());
    }

public:
    CANINO_API Registry();

    // Spawna inteiros unicos na engine ou recicla cadavers
    CANINO_API Entity CreateEntity();
    
    // Mata fisicamente e limpa as memorias de todos os componentes C++ atrelados ao ID via array pop
    CANINO_API void DestroyEntity(Entity entity);

    // Injecoes estritas de memoria de struct passiva (O(1))
    template<typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T* GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }
};

}
