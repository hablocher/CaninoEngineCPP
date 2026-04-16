#include <canino/core/memory_arena.h>
#include <canino/platform/window.h>
#include <canino/render/rhi.h>
#include <canino/ecs/registry.h>
#include <canino/math/math_ops.h>
#include <canino/core/job_system.h>
#include <iostream>
#include <vector>
#include <cstdlib>

// ===================================
// DOD SIMULATION STRUCTS
// ===================================
struct TransformComponent {
    canino::Vec4 Position; // Alinhado
    canino::Vec4 Velocity; // Alinhado
};

struct SpriteComponent {
    canino::Vec4 Color;
    float Size;
};

// ===================================
// JOB SYSTEM DATA TARGET
// ===================================
struct PhysicsStepData {
    TransformComponent* transform;
};

// O Multi-threaded Payload (Escravos chamam O(1))
void ResolvePhysicsCollisionBlock(void* data) {
    PhysicsStepData* phys = (PhysicsStepData*)data;
    
    // Adicao SIMD Ociosa
    phys->transform->Position = canino::math::Add(phys->transform->Position, phys->transform->Velocity);

    // Bouncing Box Matematica
    if (phys->transform->Position.x > 1.0f || phys->transform->Position.x < -1.0f) phys->transform->Velocity.x *= -1.0f;
    if (phys->transform->Position.y > 1.0f || phys->transform->Position.y < -1.0f) phys->transform->Velocity.y *= -1.0f;
}

// ===================================
// APPLICATION ENTRY POINT
// ===================================
int main() {
    std::cout << "[Sandbox] Iniciando Engine Integral ECS + Multithread..." << std::endl;

    canino::MemoryArena arena;
    arena.InitializeHeap(1024 * 1024 * 64);

    canino::WindowDesc desc = { "Canino Physics Simulation", 800, 600 };
    canino::Window* window = canino::PlatformCreateWindow(desc);
    
    canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window));
    canino::JobSystem_Initialize();

    std::cout << "[Sandbox] 2000 Entidades DOD Forjadas." << std::endl;

    canino::Registry registry;
    std::vector<canino::Entity> swarm;
    
    // Alocando Exercito O(1) de 2.000 Entidades
    for(int i = 0; i < 2000; ++i) {
        canino::Entity chaser = registry.CreateEntity();
        swarm.push_back(chaser);

        // Rand normalized space
        float posX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float posY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        
        float vX = (((float)rand() / RAND_MAX) * 2.0f - 1.0f) * 0.01f;
        float vY = (((float)rand() / RAND_MAX) * 2.0f - 1.0f) * 0.01f;

        float r = (float)rand() / RAND_MAX;
        float g = (float)rand() / RAND_MAX;
        float b = (float)rand() / RAND_MAX;

        registry.AddComponent<TransformComponent>(chaser, { {posX, posY, 0.0f, 0.0f}, {vX, vY, 0.0f, 0.0f} });
        registry.AddComponent<SpriteComponent>(chaser, { {r, g, b, 1.0f}, 0.02f });
    }

    // Matriz de Dados de Disparo Reutilizável (Previnindo o Malloc em Real Time)
    std::vector<PhysicsStepData> jobsPayload(2000);

    // =========================================
    // O REAL-TIME LOOP 
    // =========================================
    while (!canino::PlatformWindowShouldClose(window)) {
        canino::PlatformPumpMessages(window);

        const canino::InputState* state = canino::PlatformGetInputState(window);
        if (canino::IsKeyPressed(state, canino::CANINO_KEY_ESCAPE)) break;

        canino::JobContext physicsPassBarrier;

        // =======================
        // PHYSICS MULTI-THREADING DISPATCH
        // =======================
        for (int i = 0; i < 2000; i++) {
            TransformComponent* tf = registry.GetComponent<TransformComponent>(swarm[i]);
            if (tf) {
                jobsPayload[i].transform = tf;
                canino::JobSystem_Dispatch(&physicsPassBarrier, ResolvePhysicsCollisionBlock, &jobsPayload[i]);
            }
        }

        // Bloqueio Agressivo (Help-Stealing the Job Engine until clear)
        canino::JobSystem_Wait(&physicsPassBarrier);


        // =======================
        // RHI RENDERING PASS (Main Thread Apenas)
        // =======================
        canino::RHI_BeginFrame();
        
        // Define o Background Color Deep Dark
        canino::RenderCommand::SetClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        canino::RenderCommand::Clear();

        // Extrai DOD iterativo as Cores e Posições atuais mutadas pela Física e as manda pra Placa de Vidro
        for (int i = 0; i < 2000; i++) {
            TransformComponent* tf = registry.GetComponent<TransformComponent>(swarm[i]);
            SpriteComponent* sp = registry.GetComponent<SpriteComponent>(swarm[i]);

            if (tf && sp) {
                // Adaptando do "DrawQuad" obsoleto pro novo Shader 3D de Cubos Sólidos (com Z achatado ou transformado em pequenos blocos 3D!)
                canino::Mat4 model = canino::math::Mat4Scale(sp->Size, sp->Size, sp->Size);
                model.m[3][0] = tf->Position.x;
                model.m[3][1] = tf->Position.y;
                model.m[3][2] = 0.5f; // Joga pra frente da câmera pra não ser clipado
                
                canino::RenderCommand::DrawCubeSolid(model, sp->Color.x, sp->Color.y, sp->Color.z);
            }
        }
        
        canino::RHI_EndFrame();
    }

    canino::JobSystem_Shutdown();
    canino::PlatformDestroyWindow(window);
    return 0;
}
