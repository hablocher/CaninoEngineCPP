# 🐺 Canino Engine

**Canino Engine** é um motor gráfico/game engine 3D incrivelmente leve, arquitetado do zero e focado exclusivamente em **Data-Oriented Design (DOD)** e arquiteturas de memória **Zero-Allocation**. É um projeto idealizado e severamente arquitetado sob as fortes opiniões e direção técnica de **Hablocher**. O código fundacional, os algoritmos e a engenharia estrutural de baixo nível foram desenvolvidos através de colaboração intensa com Inteligência Artificial Avançada (Agentes da Google / LLMs).

---

## 🚀 Sobre o Projeto
O grande objetivo da Canino Engine não é competir com monstros consolidados como Unreal ou Unity, mas sim atuar como uma base educacional extrema e hardcore de engenharia C++. Focada em quem quer compreender os relógios de ciclo-perfeito da CPU e como uma GPU conversa e consome ponteiros no nível mais baixo e nativo da API do Windows (Win32 & DirectX).  

**Nossos Valores Sagrados de Programação:**
- Não usamos/abusamos da programação Orientada a Objeto clássica C++ (Sistemas densos cheios de Polimorfismo Virtual e abstração são rejeitados).
- Sem *Garbage Collection* amador.
- **ZERO `new` OU `malloc` DURANTE A SIMULAÇÃO**: Alocações heap só acontecem no Boot. As estruturas devoram Buffers únicos da Memória RAM de maneira prévia e resolvem tudo estritamente no cache local via *Offset/Pointers* usando *Memory Arenas* durante a Frame-rate!

---

## 🔥 Principais Features & Arquitetura

### 1. Sistema Matemático Base (SIMD & Alinhamento O(1))
A fundação de todo o código! Construímos cálculos matemáticos matriciais `Mat4` implementados usando diretivas de CPU Intrínsecas da Intel (128-bits `__m128` Registers). Todo cálculo de Câmera (*LookAt, Rotate, Cross Product*) foi desenvolvido em conformidade restrita com *Row-Major memory layouts* para despachar para o Shader sem overhead.

### 2. Motor Gráfico 3D (RHI DirectX 11) Avançado
O RHI atual despacha ordens atômicas direto p/ a GPU.
* **Separation of Concerns (CBO Splitting)**: Matrizes e Materiais usam Buffers de Constantes independentes para garantir ciclos de hardware ótimos. O `CBOMatrices` (Sloting b0) gerencia MVPs puristas enquanto o `CBOMaterials` (Sloting b1) administra Flags vetoriais e color settings sem truncar dados da GPU.
* **Z-Buffer / Viewport**: Pipeline de renderização embutido para desenhar Profundidade Z.
* **Wavefront Mesh Loading & Texturização**: Lê nativamente malhas geométricas complexas via Parser Nativo `OBJ` instanciando Vértices Indiceados diretamente para `D3D11_BIND_INDEX_BUFFER` e decodificando Bitmaps do disco (`stb`).

### 3. Físca Matemática 3D de Atrito Sólido (AABB Collision)
Módulo próprio de física `canino::physics`. O sistema mapeia o volume das massas no ambiente e soluciona intersseções extraindo subvetorialmente o **Minimum Translation Vector (MTV)**. Esse design matemático $O(1)$ recria perfeitamente o atrito "Wall Sliding" das paredes da engine do velho Quake de 1996 sem a necessidade complexa de bibliotecas de física colossais empacotadas artificialmente.

### 4. Leitor de Configurações Null-Alloc (`.ini`)
Um Parser manual escrito num estilo C super-leve (`canino::ConfigParser`), que lê o `canino.ini` para carregar varíaveis e bifurcar caminhos durante carregamentos `LoadFromFile`. 

### 5. Multi-Threading Job System & ECS
Task Queues assíncronas travejadas por condition variables. Entidades `std::vector` (ECS Based) operadas não como classes com setters pesados, mas alocadas contiguamente e distribuídas na pipeline CPU usando *Help-Stealing Threads*.

---

## 🎮 O Ecossistema de Demos (Sandboxes)

A Engine orquestra suas capacidades através de subprojetos nativos prontos pra compilar situados na raiz `/demos/`:

1. **FPS Demo (O Colosso)**: O Pináculo Atual do Projeto! Demo em Primeira Pessoa purista. 
   - Lida com Física AABB em Gravidade Livre para Pulos, impedindo invasão dentro dos Pilares via Matemática. 
   - Rotação matricial *Quake-Like Look FPS Lock* gerenciada por Mouse Deltas Raw (com funcionalidade ativada com parsing live do `InvertMouse X / Y` do arquivo `.ini`).
   - Usa Cross Product pra caminhar no Terreno `Right/WalkForward` e imprime *Mesh HUD overlay* estático sobreposta por matrizes na camada de Projeção sem View.
2. **Demo 1 (Sandbox de Estresse MULTI-CORE)**: Dispara instantaneamente `2000 Threads Jobs paralelos` computando física abstrata simultânea. Na etapa Render as structs estouram em O(1) transformando as posições virtuais em 2000 matrizes `Mat4` pintadas pela CPU usando `RenderCommand::DrawCube()`. 
3. **Demo 2 (Giro Cúbico)**: Um cubo primitivo de Textura Estática de Renderização isolada comprovando Perspectiva matemática sã.
4. **Texture Test**: Validador de testes QA (Automotive Unit Tests) estritamente usado pra estressar o compilador do DX contra corrupções de ponteiros C++!

---

## 💻 Como Compilar (Build/CMake)

Como parte da doutrina "Mantenha o Build System limpo", esse projeto não suja sua branch com setups temporários do host IDE local. Exige a instalação base do **CMake** na sua máquina além dos compiladores clássicos MSVC do Windows. 

1. Git Clone este repositório.
2. Abra o Shell (Powershell de preferência).
3. Invoque a montagem nativa da máquina pela Engine Geradora:
   ```shell
   cmake -B build
   ```
4. Submeta a orquestração do Build invocando todas as bibliotecas de compêndio Linker e Subdemos na máxima potência disponível estriando a flag release C++ (-O2 equivalente) ou mantenho o conforto de Assertions Debug:
   ```shell
   cmake --build build --config DEBUG
   ```

*(Certifique que seu Visual Studio / Windows SDK contém o Pack nativo Desktop C++).*

---

## 🕹 Como Rodar (Run) o Jogo

Em nome da facilidade e arquitetura limpa, o CMake proíbe DLLs soltas na pasta fonte! Todas as resoluções operam agrupadas em diretório destino.

Basta entrar nas Demo Sandboxes prontas:

```shell
.\build\bin\DEBUG\FPSDemo.exe
```

**[ 🎮 Controlando o FPS Demo ]** 
* Mouse vira o Pescoço. (*Altere os Inverts em `canino.ini` e teste o Reload reativo*)
* **`W, A, S, D`** Desliza e atrita com o Terreno livre numa velocidade fixa `proposedVelocity`.
* **`ESPAÇO`**: Salta acionando aceleração de Y Invertido e sofre degradação em `player.velocityY` até interceptar um obstáculo superior e o código detectar O(1) a volta ao Status de _Grounded_.
* Use **`ESCAPE (ESC)`** a qualquer segundo para anular violentamente o ECS do Cóptero Multithreading, Matar o Contexto do DirectX nativo em 0 bytes Leak, e Destruir a Janela Windows32 pela API limpa do SO. 
