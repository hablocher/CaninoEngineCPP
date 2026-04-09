#include <canino/core/core.h>
#include <canino/platform/window.h>
#include <canino/platform/input.h>
#include <canino/render/rhi.h>
#include <canino/render/mesh_loader.h>
#include <canino/math/math_ops.h>
#include <canino/ecs/ecs.h>
#include <iostream>
#include <vector>

// Componentes do ECS
struct TransformComponent {
    canino::Vec3 position;
    canino::Vec3 scale;
};
// Componentes ECS de Modelos
struct TexturedComponent {
    void* TextureId;
};
struct PlayerComponent {
    float velocityY;
    bool isGrounded;
};

// Variaveis de Visão de Câmera (FPS Viewport)
float g_Yaw = 0.0f;
float g_Pitch = 0.0f;

int main() {
    canino::WindowDesc desc = { "Canino Engine - Quake FPS Sandbox", 1280, 720 };
    canino::Window* window = canino::PlatformCreateWindow(desc);
    if (!window) return -1;
    
    canino::PlatformLockCursor(window, true);
    canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window));

    // Terreno Massivo
    TransformComponent floorTransform = { {0.0f, -0.5f, 0.0f}, {100.0f, 1.0f, 100.0f} };
    void* texFloor = canino::RenderCommand::CreateTexture("assets/floor_texture.jpg");

    // Pilares via ECS
    std::vector<TransformComponent> pillarsT;
    for (int i = -4; i <= 4; i += 4) {
        for (int j = -4; j <= 4; j += 4) {
            if (i == 0 && j == 0) continue;
            pillarsT.push_back({ {(float)i * 5.0f, 1.0f, (float)j * 5.0f}, {1.5f, 4.0f, 1.5f} });
        }
    }
    void* texPillar = canino::RenderCommand::CreateTexture("assets/pillar_texture.jpg");

    // Weapon Asset Pipeline Extrema
    TransformComponent gunTransform = { {0.5f, -0.4f, 1.0f}, {0.2f, 0.2f, 1.0f} }; // Posicionamento na UI Câmera
    void* texGun = canino::RenderCommand::CreateTexture("assets/gun_texture.jpg");
    
    canino::MeshDataRaw gunRaw = canino::MeshLoader::LoadOBJ("assets/gun.obj");
    void* gunMesh = nullptr;
    if (!gunRaw.Vertices.empty()) {
        gunMesh = canino::RenderCommand::CreateMesh(gunRaw.Vertices.data(), gunRaw.Vertices.size(), gunRaw.Indices.data(), gunRaw.Indices.size());
    }

    // Lógica Física Hardcodada Direcional do Jogador
    canino::Vec3 eye = { 0.0f, 2.0f, 0.0f }; // Altura do Player
    PlayerComponent player = { 0.0f, false };

    auto proj = canino::math::Mat4Perspective(3.1415f / 3.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

    while (!canino::PlatformWindowShouldClose(window)) {
        canino::PlatformPumpMessages(window);
        const canino::InputState* input = canino::PlatformGetInputState(window);

        if (canino::IsKeyDown(input, canino::CANINO_KEY_ESCAPE)) break;

        // ====== 1. ROTINA DE MOUSE (QUAKE LOCK FPS) ======
        float mouseSensitivity = 0.003f;
        g_Yaw -= input->MouseDeltaX * mouseSensitivity; // Movimento invés pra mapear Rotação LHS
        g_Pitch -= input->MouseDeltaY * mouseSensitivity;

        // Clamping agressivo de quebra de pescoço (+- 89 graus)
        if (g_Pitch > 1.55f) g_Pitch = 1.55f;
        if (g_Pitch < -1.55f) g_Pitch = -1.55f;

        // Variavéis Derivadas Matriciais Quake-Like do Front Engine
        canino::Vec3 forward;
        forward.x = sinf(g_Yaw) * cosf(g_Pitch);
        forward.y = -sinf(g_Pitch); // Subtrai a visão pelo Drag MouseY
        forward.z = cosf(g_Yaw) * cosf(g_Pitch);
        forward = canino::math::Vec3Normalize(forward);

        // Movimentação do Eixo Paralelo de Caminhada (Sem Flutuar/Voar na Rotação FPS, Z e X puros)
        canino::Vec3 walkForward = canino::math::Vec3Normalize({forward.x, 0.0f, forward.z});
        canino::Vec3 up = { 0.0f, 1.0f, 0.0f };
        canino::Vec3 right = canino::math::Vec3Normalize(canino::math::Vec3Cross(up, walkForward));

        // ====== 2. WASD MOVEMENT LOGIC ======
        float currentSpeed = 0.025f; // Walk speed fixa FPS
        if (canino::IsKeyDown(input, canino::CANINO_KEY_W)) eye = canino::math::Vec3Add(eye, canino::math::Vec3Mul(walkForward, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_S)) eye = canino::math::Vec3Sub(eye, canino::math::Vec3Mul(walkForward, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_D)) eye = canino::math::Vec3Add(eye, canino::math::Vec3Mul(right, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_A)) eye = canino::math::Vec3Sub(eye, canino::math::Vec3Mul(right, currentSpeed));

        // JUMP AND GRAVITY PURE
        player.velocityY -= 0.003f; // Gravity Accel
        if (player.isGrounded && canino::IsKeyDown(input, canino::CANINO_KEY_SPACE)) {
            player.velocityY = 0.06f; // Jump Force
            player.isGrounded = false;
        }

        eye.y += player.velocityY;

        // Collision Check no Terreno Basico do 100m²
        if (eye.y < 2.0f) { // Ground Level View
            eye.y = 2.0f;
            player.velocityY = 0.0f;
            player.isGrounded = true;
        } else {
            player.isGrounded = false;
        }

        // View Matrix
        canino::Mat4 view = canino::math::Mat4LookAt(eye, forward, up);

        // ====== 3. RENDERIZAÇÃO MASSIVA ======
        canino::RHI_BeginFrame();
        canino::RenderCommand::SetClearColor(0.5f, 0.8f, 0.9f, 1.0f); // Céu Azulado
        canino::RenderCommand::Clear();

        // Pass 1: Floor Textured
        canino::Mat4 floorModel = canino::math::Mat4Scale(floorTransform.scale.x, floorTransform.scale.y, floorTransform.scale.z);
        floorModel.m[3][0] = floorTransform.position.x;
        floorModel.m[3][1] = floorTransform.position.y;
        floorModel.m[3][2] = floorTransform.position.z;
        canino::Mat4 floorMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(floorModel, view), proj);
        canino::RenderCommand::DrawCube(floorMVP, texFloor);

        // Pass 2: Pillars Textured
        for (size_t i = 0; i < pillarsT.size(); ++i) {
            canino::Mat4 pModel = canino::math::Mat4Scale(pillarsT[i].scale.x, pillarsT[i].scale.y, pillarsT[i].scale.z);
            pModel.m[3][0] = pillarsT[i].position.x;
            pModel.m[3][1] = pillarsT[i].position.y;
            pModel.m[3][2] = pillarsT[i].position.z;
            canino::Mat4 pMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(pModel, view), proj);
            canino::RenderCommand::DrawCube(pMVP, texPillar);
        }

        // Pass 3: HUD Gun First Person Overlay
        // Desconsideramos completamente a matriz View e jogamos ela numa Projection Absoluta Centralizada!! (UI Space)
        canino::Mat4 gunModel = canino::math::Mat4Scale(gunTransform.scale.x, gunTransform.scale.y, gunTransform.scale.z);
        canino::Mat4 gunRotX = canino::math::Mat4RotateX(0.1f);  // Inclinada pra camera minimamente
        canino::Mat4 gunRotY = canino::math::Mat4RotateY(-0.1f); // De ladinho esteticamente
        
        canino::Mat4 gunRotFull = canino::math::Mat4Multiply(gunRotY, gunRotX);
        canino::Mat4 gunTransformed = canino::math::Mat4Multiply(gunModel, gunRotFull);
        
        canino::Mat4 gunOverlayView = canino::math::Mat4Translate(gunTransform.position.x, gunTransform.position.y, gunTransform.position.z);
        canino::Mat4 gunMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(gunTransformed, gunOverlayView), proj);
        
        // Se conseguimos parsear a Mesh na VRAM, desenhamos com Textura!
        if (gunMesh) {
            canino::RenderCommand::DrawMesh(gunMesh, gunMVP, texGun);
        } else {
            canino::RenderCommand::DrawCubeSolid(gunMVP, 0.8f, 0.2f, 0.2f);
        }


        canino::RHI_EndFrame();
        canino::PlatformUpdateInputState(window);
    }

    canino::RHI_Shutdown();
    canino::PlatformDestroyWindow(window);
    return 0;
}
