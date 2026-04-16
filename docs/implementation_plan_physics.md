# 3D AABB Physics & Collision System

Este plano detalha a criação do módulo nativo matemático de física em 3D da Canino Engine, focando em Axis-Aligned Bounding Boxes (AABB), e como ele será integrado na demo de FPS.

> [!NOTE]
> **Status:** Aprovado
> O design matemático escolhido foi o Sliding simples em eixos X/Y/Z para garantir performance ótima e manter a agilidade clássica Quake-like sem se atar às resoluções continuas e dispendiosas do Swept-Volume agora.

## Proposed Changes

### [Component Name] Engine Core - Physics API
Criar a fundação estrutural para checagem e resolução de intersecção em 3D entre caixas invisíveis para criar barreiras sólidas.

#### [NEW] [physics.h](file:///e:/Code/CPP/CaninoEngine/engine/src/canino/physics/physics.h)
- Struct `AABB { Vec3 min, max; }`.
- Função auxiliar `AABBFromTransform(pos, scale)`.
- Struct `CollisionHit { bool hasHit; Vec3 normal; float depth; }`.
- Funções base `CheckAABB(AABB, AABB)` e `ResolveCollision(AABB dyna, AABB stat)`.

#### [NEW] [physics.cpp](file:///e:/Code/CPP/CaninoEngine/engine/src/canino/physics/physics.cpp)
- Implementação matemática orientada a dados da intersecção AABB-AABB calculando profundidade mínima e vetor normal de expulsão (para simular a física de deslize "Quake-like" nas paredes).

#### [MODIFY] [CMakeLists.txt](file:///e:/Code/CPP/CaninoEngine/engine/CMakeLists.txt)
- Incluir `physics.cpp` no compêndio `CaninoEngine`.

---

### [Component Name] FPS Demo
Conectar o motor de matriz de colisões estáticas ao gameplay prático de cenário do player.

#### [MODIFY] [main.cpp](file:///e:/Code/CPP/CaninoEngine/demos/fpsdemo/src/main.cpp)
- Inicializar `std::vector<canino::AABB> worldColliders` povoado pelas áreas de tamanho do chão e dos pilares.
- Construir o HULL limitador físico do jogador (`playerAABB`) baseado no `eye`.
- Durante a gravidade e o input (WASD), injetar a chamada ao resolvedor colisional. Se bater numa parede (ex: Pilar), expulsar a câmera para deslizar. Se bater no chão, setar `isGrounded = true`.

## Verification Plan
### Manual Verification
- Compilar `FPSDemo.exe`.
- Andar contra os pilares do salão repetidas vezes até validar ausência de clipping.
- Pular (Espaço) e ver as colisões tridimensionais operarem simultaneamente.
