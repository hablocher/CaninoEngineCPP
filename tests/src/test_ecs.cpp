#include "test_core.h"
#include <canino/ecs/registry.h>
#include <canino/math/math_ops.h>
#include <iostream>

struct TransformTestComponent {
    canino::Vec4 Position;
    float Velocity;
};

struct PhysicsTestComponent {
    canino::Vec4 Mass; // Usando os structs brutais 16-align intrinsecos
};

static bool Test_ECS_RegistryTicking() {
    canino::Registry registry;

    // Criando 3 entidades escuras em O(1)
    canino::Entity ent1 = registry.CreateEntity();
    canino::Entity ent2 = registry.CreateEntity();
    canino::Entity ent3 = registry.CreateEntity();

    CANINO_EXPECT(ent1 == 0 && ent2 == 1 && ent3 == 2); // Sequential

    // Injetando Matrizes de Componetizacao Sparse Set
    registry.AddComponent<TransformTestComponent>(ent1, { {0,0,0,1}, 10.0f });
    registry.AddComponent<TransformTestComponent>(ent2, { {1,1,1,1}, 20.0f });

    // Consulta Absoluta de CTE Hash
    TransformTestComponent* tf2 = registry.GetComponent<TransformTestComponent>(ent2);
    CANINO_EXPECT(tf2 != nullptr);
    CANINO_EXPECT(tf2->Velocity == 20.0f);

    // Verificando que Entidade NULA nao tem Componente associado (Buraco vazio na Cache L1 Sparse)
    TransformTestComponent* tf3 = registry.GetComponent<TransformTestComponent>(ent3);
    CANINO_EXPECT(tf3 == nullptr);

    return true;
}

static bool Test_ECS_SwapAndPop() {
    canino::Registry registry;
    
    canino::Entity root = registry.CreateEntity();
    canino::Entity enemy = registry.CreateEntity();
    canino::Entity leaf = registry.CreateEntity();

    registry.AddComponent<PhysicsTestComponent>(root, { {100, 0, 0, 0} });
    registry.AddComponent<PhysicsTestComponent>(enemy, { {500, 0, 0, 0} });
    registry.AddComponent<PhysicsTestComponent>(leaf, { {1, 0, 0, 0} });

    // Matamos a Entidade INTERMEDIÁRIA do pacote C++ de memoria Ram (Fragmentação induzida DOD)
    registry.DestroyEntity(enemy);

    // Em OOP padrao, list.erase() pularia os arrays na CPU usando for-loops caros.
    // Em Sparse Set O(1), DestroyEntity bate em RemoveData() que faz the Swap and pop.
    // O leaf (componente Physics do Leaf) migrou silenciosamente pras areas densas antigas do Enemy pra colar o Array L1!!!

    PhysicsTestComponent* pfRoot = registry.GetComponent<PhysicsTestComponent>(root);
    PhysicsTestComponent* pfLeaf = registry.GetComponent<PhysicsTestComponent>(leaf);

    CANINO_EXPECT(pfRoot != nullptr && pfRoot->Mass.x == 100);
    CANINO_EXPECT(pfLeaf != nullptr && pfLeaf->Mass.x == 1);
    
    // O Enemy ja devia ser Nullptr O(1) na Lookup de Sparse
    CANINO_EXPECT(registry.GetComponent<PhysicsTestComponent>(enemy) == nullptr);

    return true;
}

bool RunECSTests() {
    CANINO_RUN_TEST(Test_ECS_RegistryTicking);
    CANINO_RUN_TEST(Test_ECS_SwapAndPop);
    return true;
}
