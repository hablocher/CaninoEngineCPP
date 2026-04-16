#include <canino/core/core.h>
#include <canino/platform/window.h>
#include <canino/platform/input.h>
#include <canino/core/config_parser.h>
#include <canino/render/rhi.h>
#include <canino/render/mesh_loader.h>
#include <canino/math/math_ops.h>
#include <canino/ecs/ecs.h>
#include <canino/physics/physics.h>
#include <iostream>
#include <vector>

// -------------------------------------------------------------
// ESTRUTURAS DE DADOS DO JOGO (Base do Entity Component System)
// -------------------------------------------------------------

// O TransformComponent guarda a Posição e Escala de um objeto estático no mundo 3D.
struct TransformComponent {
    canino::Vec3 position;
    canino::Vec3 scale;
};

// O PlayerComponent gerencia o estado da física e inércia ativa do jogador.
struct PlayerComponent {
    float velocityY;    // A velocidade de queda por causa da Gravidade
    bool isGrounded;    // Verdadeiro se o jogador estiver pisando no chão firme
};

// -------------------------------------------------------------
// VARIÁVEIS GLOBAIS DE CÂMERA (Visão Livre)
// -------------------------------------------------------------
float g_Yaw = 0.0f;   // Rotação horizontal (Olhar para Esquerda/Direita)
float g_Pitch = 0.0f; // Rotação vertical (Olhar para Cima/Baixo)

int main() {
    // =========================================================
    // 1. INICIALIZAÇÃO DA JANELA E DO SISTEMA GRÁFICO (RHI)
    // =========================================================
    canino::WindowDesc desc = { "Canino Engine - Quake FPS Sandbox", 1280, 720 };
    canino::Window* window = canino::PlatformCreateWindow(desc);
    if (!window) return -1;
    
    canino::PlatformLockCursor(window, true); // Prende e esconde o ponteiro do mouse na janela (Imersão 3D)
    canino::RHI_Initialize(canino::PlatformGetNativeWindowHandle(window)); // Inicia a Placa de Vídeo Host (DirectX/OpenGL/etc)

    // =========================================================
    // 2. LEITURA DE CONFIGURAÇÕES (Lendo dados do disco - canino.ini)
    // =========================================================
    canino::ConfigParser::LoadFromFile("canino.ini");
    bool invertMouseX = canino::ConfigParser::GetBool("InvertMouseX", false);
    bool invertMouseY = canino::ConfigParser::GetBool("InvertMouseY", false);

    // =========================================================
    // 3. CRIAÇÃO DO MUNDO (Geometria Estática e Texturas na VRAM)
    // =========================================================
    
    // Inicialização do Chão (Um cubo esticado 100x100)
    TransformComponent floorTransform = { {0.0f, -0.5f, 0.0f}, {100.0f, 1.0f, 100.0f} };
    void* texFloor = canino::RenderCommand::CreateTexture("assets/floor_texture.jpg"); // Textura copiada da RAM para a Placa de Vídeo

    // Geração procedural simples: Array de Pilares espalhados numa malha 3x3 no mundo
    std::vector<TransformComponent> pillarsT;
    for (int i = -4; i <= 4; i += 4) {
        for (int j = -4; j <= 4; j += 4) {
             // Deixa o centro (0,0) livre para o jogador nascer sem ficar preso dendro de um pilar!
            if (i == 0 && j == 0) continue;
            pillarsT.push_back({ {(float)i * 5.0f, 1.0f, (float)j * 5.0f}, {1.5f, 4.0f, 1.5f} });
        }
    }
    void* texPillar = canino::RenderCommand::CreateTexture("assets/pillar_texture.jpg");

    // HUD do Jogador (Carregador de Modelos Genuínos Wavefront OBJ)
    // Definimos sua posição colada no espaço de Câmera UI (X=0.5 Canto direito, Z=1.0 Profundidade)
    TransformComponent gunTransform = { {0.5f, -0.4f, 1.0f}, {0.2f, 0.2f, 1.0f} };
    void* texGun = canino::RenderCommand::CreateTexture("assets/gun_texture.jpg");
    
    canino::MeshDataRaw gunRaw = canino::MeshLoader::LoadOBJ("assets/gun.obj");
    void* gunMesh = nullptr;
    if (!gunRaw.Vertices.empty()) {
        // Envia geometria O(1) de vértices brutos pra memória blindada do DirectX (VertexBuffers locais da VRAM)
        gunMesh = canino::RenderCommand::CreateMesh(gunRaw.Vertices.data(), gunRaw.Vertices.size(), gunRaw.Indices.data(), gunRaw.Indices.size());
    }

    // =========================================================
    // 4. MÚSCULO DAS COLISÕES FÍSICAS MATH (Estruturas AABB)
    // =========================================================
    // O array worldColliders atua como a infraestrutura estática invencível da dimensão.
    std::vector<canino::AABB> worldColliders;
    
    // Cadastra o volume invisível do chão (Para impedir queda no Limbo)
    worldColliders.push_back(canino::physics::AABBFromTransform(floorTransform.position, floorTransform.scale));
    
    // Engole cada pilar gerado no for-loop como pedaços colossais de pedra no mapa
    for (const auto& p : pillarsT) {
        worldColliders.push_back(canino::physics::AABBFromTransform(p.position, p.scale));
    }

    // =========================================================
    // 5. INICIALIZAÇÃO E BOOT DO CASTER JOGADOR
    // =========================================================
    canino::Vec3 eye = { 0.0f, 2.0f, 0.0f }; // O olho físico é nosso ponto focal geométrico primário.
    PlayerComponent player = { 0.0f, false };

    // Matriz Estática de Projeção da Lente 
    // Mapeia coordenadas mundiais pro monitor (60º Graus de FOV, 16:9 aspecto, Rendering Bounds: 0.1m ~ 1000m)
    auto proj = canino::math::Mat4Perspective(3.1415f / 3.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);


    // =========================================================
    // ================== REAL-TIME RENDER LOOP ================
    // =========================================================
    while (!canino::PlatformWindowShouldClose(window)) {
        
        // Puxa e Resolve dados atrasados da placa-mãe (Garante que a UI não trave)
        canino::PlatformPumpMessages(window);
        const canino::InputState* input = canino::PlatformGetInputState(window);

        if (canino::IsKeyDown(input, canino::CANINO_KEY_ESCAPE)) break; // Comando Escape Abort

        // --------------------------------------------
        // FASE 1: LÓGICA DE VISÃO (QUAKE CAMERA AIM)
        // --------------------------------------------
        float mouseSensitivity = 0.003f;
        
        // Yaw (Olhar esquerda/direita)
        if (invertMouseX) {
            g_Yaw += input->MouseDeltaX * mouseSensitivity;
        } else {
            g_Yaw -= input->MouseDeltaX * mouseSensitivity; 
        }

        // Pitch (Olhar cima/baixo)
        if (invertMouseY) {
            g_Pitch += input->MouseDeltaY * mouseSensitivity;
        } else {
            g_Pitch -= input->MouseDeltaY * mouseSensitivity;
        }

        // Limita a rotação do pescoço pra não causar um giro 360 e deformar Eixo (Clamping de +- 89 Graus em Radianos)
        if (g_Pitch > 1.55f) g_Pitch = 1.55f;
        if (g_Pitch < -1.55f) g_Pitch = -1.55f;

        // Tramos a Direção Matemática do Rosto (Forward Vector) usando trigonometria pura
        canino::Vec3 forward;
        forward.x = sinf(g_Yaw) * cosf(g_Pitch);
        forward.y = -sinf(g_Pitch); 
        forward.z = cosf(g_Yaw) * cosf(g_Pitch);
        forward = canino::math::Vec3Normalize(forward); 

        // Derivamos vetores de "Caminhar pra Frente Plana (WalkForward)" (ignorando se o mouse olha pro teto) 
        // e o Vetor Tangente "Direita (Right)" pelo truque de Cross-Product Produto Vetorial!
        canino::Vec3 walkForward = canino::math::Vec3Normalize({forward.x, 0.0f, forward.z});
        canino::Vec3 up = { 0.0f, 1.0f, 0.0f };
        canino::Vec3 right = canino::math::Vec3Normalize(canino::math::Vec3Cross(up, walkForward));

        // --------------------------------------------
        // FASE 2: FÍSICA CINÉTICA E IMPULSO DE TECLADO
        // --------------------------------------------
        float currentSpeed = 0.025f; // Constante de aceleração global da frame rate
        canino::Vec3 proposedVelocity = {0,0,0}; // Quantidade bruta de vetores que QUEREMOS nos movimentar frame ideal

        if (canino::IsKeyDown(input, canino::CANINO_KEY_W)) proposedVelocity = canino::math::Vec3Add(proposedVelocity, canino::math::Vec3Mul(walkForward, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_S)) proposedVelocity = canino::math::Vec3Sub(proposedVelocity, canino::math::Vec3Mul(walkForward, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_D)) proposedVelocity = canino::math::Vec3Add(proposedVelocity, canino::math::Vec3Mul(right, currentSpeed));
        if (canino::IsKeyDown(input, canino::CANINO_KEY_A)) proposedVelocity = canino::math::Vec3Sub(proposedVelocity, canino::math::Vec3Mul(right, currentSpeed));

        // PULO E GRAVIDADE REALISTA (Newtons por tick)
        player.velocityY -= 0.003f; // Decremento contínuo da inércia
        if (player.isGrounded && canino::IsKeyDown(input, canino::CANINO_KEY_SPACE)) {
            player.velocityY = 0.06f; // Força explosiva y+ (Foguete Empuxo)
            player.isGrounded = false;
        }

        proposedVelocity.y = player.velocityY;
        
        // Injetamos e construimos o Ponto Vitual (Proposed Eye), pra simularmos qual lugar vamos colidir "no instante futuro".
        canino::Vec3 proposedEye = canino::math::Vec3Add(eye, proposedVelocity);

        // Constroi a Caixa Invisivel (Corpo Físico do Jogador) centrado em "ProposedEye"
        canino::Vec3 playerScale = { 0.6f, 1.8f, 0.6f }; // Medidas humanoides 1.8m
        canino::AABB playerHull = canino::physics::AABBFromTransform(proposedEye, playerScale);

        // Limpa flag de chão da engine, esperando ver se cairemos livres pela eternidade.
        player.isGrounded = false;

        // --------------------------------------------
        // FASE 3: ESQUADRÃO ANTI-CLIPPING (Resolvedor AABB O(1))
        // --------------------------------------------
        for (const auto& wall : worldColliders) {
            canino::CollisionHit hit = canino::physics::ResolveAABB(playerHull, wall);
            if (hit.hasHit) { 
                
                // Nós invadimos uma parede estática. Acionamos o Vetor MTV de Expulsão Imediata (Deslize):
                // Adicionar o inverso da invasão nos teleporta pra superfície tocada! Resulta num "Slide" liso.
                proposedEye.x += hit.penetration.x;
                proposedEye.y += hit.penetration.y;
                proposedEye.z += hit.penetration.z;

                // Resistência Normal do Impacto Chão/Teto
                if (hit.penetration.y > 0.0f) { // Fomos expulsos verticalmente CIMA (+y). Bateu o pé!
                    player.isGrounded = true;   
                    player.velocityY = 0.0f;    
                } else if (hit.penetration.y < 0.0f) { // Bateu a cabeça!
                    player.velocityY = 0.0f;    
                }

                // Recorrige a caixa local do corpo para não testar batidas em loop corrompidas e se teleportar varando paredes próximas
                playerHull = canino::physics::AABBFromTransform(proposedEye, playerScale);
            }
        }

        // CONFIRMA A JOGADA. Move a Câmera definitivamente ao redor deste ponto aprovado pelas Leis Físicas!
        eye = proposedEye;

        // =========================================================================
        // SUBSISTEMA GRÁFICO (PIPELINE Z-BUFFER DE 3 CAMADAS)
        // =========================================================================

        // Matriz Dinâmica da Câmera Look At View (Reverso-Espelhado para enviar a Geometria global)
        canino::Mat4 view = canino::math::Mat4LookAt(eye, forward, up);

        canino::RHI_BeginFrame();
        canino::RenderCommand::SetClearColor(0.5f, 0.8f, 0.9f, 1.0f); // Tinta Azul do Céu Eterno (Backdrop)
        canino::RenderCommand::Clear(); // Reseta os pixels e DepthBuffer (Evita rastros fantasmas da Fps)

        // LAYER 1: Desenhando Chão
        canino::Mat4 floorModel = canino::math::Mat4Scale(floorTransform.scale.x, floorTransform.scale.y, floorTransform.scale.z);
        floorModel.m[3][0] = floorTransform.position.x;
        floorModel.m[3][1] = floorTransform.position.y;
        floorModel.m[3][2] = floorTransform.position.z;
        
        // Multiplicação Algébrica Final Vertex Matrix: M * V * P
        canino::Mat4 floorMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(floorModel, view), proj);
        canino::RenderCommand::DrawCube(floorMVP, texFloor); 

        // LAYER 2: Pilastras do Templo Clássico D3D
        for (size_t i = 0; i < pillarsT.size(); ++i) {
            canino::Mat4 pModel = canino::math::Mat4Scale(pillarsT[i].scale.x, pillarsT[i].scale.y, pillarsT[i].scale.z);
            pModel.m[3][0] = pillarsT[i].position.x;
            pModel.m[3][1] = pillarsT[i].position.y;
            pModel.m[3][2] = pillarsT[i].position.z;
            canino::Mat4 pMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(pModel, view), proj);
            canino::RenderCommand::DrawCube(pMVP, texPillar);
        }

        // LAYER 3: Armas Físicas Customizadas (Gun Overlay View)
        // OBS: Descartamos a matriz CAMERA VIRTUAL para forçar o Desenho HUD ser imune a onde estamos andando.
        canino::Mat4 gunModel = canino::math::Mat4Scale(gunTransform.scale.x, gunTransform.scale.y, gunTransform.scale.z);
        canino::Mat4 gunRotX = canino::math::Mat4RotateX(0.1f);  
        canino::Mat4 gunRotY = canino::math::Mat4RotateY(-0.1f); 
        
        canino::Mat4 gunRotFull = canino::math::Mat4Multiply(gunRotY, gunRotX); // Fusão Axial YX
        canino::Mat4 gunTransformed = canino::math::Mat4Multiply(gunModel, gunRotFull);
        
        canino::Mat4 gunOverlayView = canino::math::Mat4Translate(gunTransform.position.x, gunTransform.position.y, gunTransform.position.z);
        // Despacha puramente em Perspectiva Absoluta Imóvel (Como uma UI 3D): Overlay M * V(False) * P(Real)
        canino::Mat4 gunMVP = canino::math::Mat4Multiply(canino::math::Mat4Multiply(gunTransformed, gunOverlayView), proj);
        
        if (gunMesh) {
            // Emite as centenas de triângulos na VRAM e Mapeia SRV JPG para as cores!
            canino::RenderCommand::DrawMesh(gunMesh, gunMVP, texGun);
        } else {
            // Bloco reserva caso o arquivo do OBJ perca a linkagem C++
            canino::RenderCommand::DrawCubeSolid(gunMVP, 0.8f, 0.2f, 0.2f);
        }

        canino::RHI_EndFrame(); // Present () Call -> Arranca Pointers e Joga Imagem pra Tela L-Monitor VSync!
        canino::PlatformUpdateInputState(window); // Fecha cache mouse pos
    }

    // 6. GARBAGE COLLECTION E DESTRUICAO O(1)
    canino::RHI_Shutdown();
    canino::PlatformDestroyWindow(window);
    return 0;
}
