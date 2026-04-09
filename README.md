# 🐺 Canino Engine

**Canino Engine** é um motor gráfico/game engine 3D incrivelmente leve, arquitetado do zero e focado exclusivamente em **Data-Oriented Design (DOD)** e arquiteturas de memória **Zero-Allocation**. É um projeto idealizado e severamente arquitetado sob as fortes opiniões e direção técnica de **Hablocher**. O código fundacional, os algoritmos e a engenharia estrutural de baixo nível foram desenvolvidos através de colaboração intensa com Inteligência Artificial Avançada (Agentes da Google / LLMs).

---

## 🚀 Sobre o Projeto
O grande objetivo da Canino Engine não é competir com monstros consolidados como Unreal ou Unity, mas sim atuar como uma base educacional extrema e hardcore de engenharia C++. Focada em quem quer compreender os relógios de ciclo-perfeito da CPU e como uma GPU conversa e consome ponteiros no nível mais baixo e nativo da API do Windows (Win32 & DirectX).  

**Nossos Cânios Sagrados de Programação:**
- Não usamos/abusamos da programação Orientada a Objeto clássica C++ (Sistemas densos cheios de Polimorfismo Virtual e abstração são rejeitados).
- Sem *Garbage Collection* amador.
- **ZERO `new` OU `malloc` DURANTE A SIMULAÇÃO**: Alocações heap só acontecem no Boot. As estruturas devoram Buffers únicos da Memória RAM de maneira prévia e resolvem tudo estritamente no cache local via *Offset/Pointers* usando *Memory Arenas* durante a Frame-rate!

---

## 🔥 Principais Features & Arquitetura

### 1. Memory Arenas (Zero-Allocation)
Toda a memória é separada massivamente no início do escopo principal (`Upfront Allocation`). Componentes injetando alocadores unificados que evitam em 100% que as Listas Ligadas tradicionais matem e fragmentem os recursos do PC do usuário ou destruam O Cache do processador. 

### 2. Entity Component System (ECS Baseado em Sparse Sets)
Focado integralmente no Data-Oriented Design. Os dados das entidades não são trancados em encapsulamentos pesados como Classes GameObjects. Temos um ECS purista, armazenando as listas (`Transform`, `Physics`) de forma em pacotes de Cache Arrays Paralelos alinhados contíguos (Contiguous Blocks). 

### 3. Multi-Threading & Job System Puro (C++20)
*Task Queues* assíncronas projetadas para escalabilidade. Disparamos jobs com forte paralelismo de dados controlados matematicamente pelos sub-módulos para distribuir ociosidades pesadas em Múltiplos Cores (Núcleos Físicos de Processador) travando os semáforos com `std::condition_variable`.

### 4. Renderização 3D Direta (RHI Agnóstico)
O Sub-sistema RHI (*Render Hardware Interface*) atua baseada numa V-Table (Tabela de Ponteiros de Função no velho estilo do C puro) preparada para receber injeção transparente de Vulkan, DX12 ou OpenGL.
* **Backend Windows Focado em DirectX 11 (ativo)**: Implantações completas com Constant Buffers (CBO), Index/Vertex Arrays e Shaders programáveis de tempo de execução (`D3DCompiler`). Z-Buffer/Depth Stencil garantindo a oclusão das faces no Espaço 3D e Rasterização manual do Pipeline controlando Vertex Winding Order e Cullings pela placa de hardware!
* **Matemática SSE/SIMD**: Uma biblioteca nativa customizada construindo matemática Matricial (`Mat4`), construída desde a raiz com intrínsicos Intel Registradores 128-bits `__m128`. Processando matrizes de *Perspective, Rotate, Translate, Identidade* no nível base e repassando em conformidade restrita com *Row-Major memory layouts* para dentro da HLSL.

---

## 🎮 Demos Incorporados

Dentro do diretório `/demos/`, criamos escopos limpos isolados via Sandboxes compiladas e executáveis que consomem a Engine base como um ecossistema. Modifique-os livres de receios:

1. **Demo 1**: Aplicação de Provas e Testes. Atesta e visualiza a submissão Paralela do ECS em junção de centenas de objetos.  
2. **Demo 2**: A Ascensão Tridimensional! Um Sandbox consumindo primitivas nativas da GPU pra rotacionar um modelo cúbico AAA com dimensões base predefinidas perfeitamente (Raio Unitário Fixo), carregando Imagens como Textura Direta, com cálculos perfeitamente imunes à quebra de proporções Matrix/Perspective Frustum. Interativa usando Sistema Integrado de Keyboard Raw Input.

---

## 🛠 Tecnologias e Estaque

* **Linguagem**: C++20.
* **Design/Compilação**: Módulo de Projetos Moderno (CMake modular de Multi-Targets). Modos Release rigorosos compilando com `-O3/-O2`.
* **API de Interface Base**: Win32 OS (Capturas nativas dos Loops de Windows e Input Events `WM_KEYDOWN/UP`).
* **Gráficos**: APIs Hardware Nativo DXGI/D3D11 e shaders embutidos puristas HLSL.
* **Carregamento Textilar**: Assinado com `stb_image.h` puramente isolado para empurrar decodificação JPEGs pra GPU zero-overhead.
* **Ferramenta de Criação IA**: Arquitetado por *Google AI Agents/Modelos LLMs* atuando inteiramente como codificador do framework e operário do Teclado mas respondendo à Direção Arquiteta exata e intencional imposta pelo Líder.

---

## 💻 Como Empacotar & Compilar (Build/CMake)

Como parte da doutrina "Mantenha o Build System limpo", esse projeto nunca sobe dependências do Visual Studio sujas na branch principal (`.nmake`, `.sln`, `.vcxproj`). Tudo é gerado fresquinho na máquina de quem baixa! Requer **CMake (v3.20+)** e as Bibliotecas do **Visual Studio 2022 C++**.

1. Git Clone este repositório no seu local predileto.
2. Abra o Shell na pasta raiz `CaninoEngine`.
3. Invoque o Gerador para orquestrar os Makefiles alvos para um ambiente em isolamento na pasta "Build":
   ```shell
   cmake -B build
   ```
4. Recomende a Compilação do Linker/Engine nativa e as Sandboxes com o máximo de Otimização Estável de Debugging nativa: 
   ```shell
   cmake --build build --config DEBUG
   ```
*(Se quiser ver o ECS batendo sua velocidade máxima O2 de otimização troque por `RELEASE`)*.

> **Dicas para o CMake/Windows**: Esta Engine está programada para puxar o `d3d11.lib` localmente do Sistema. Para que compile nativo de primeira no Windows assegure que ao instalar o VS2022 o pacote básico "Windows 10/11 SDK C++ Desktop Development" esteja setado no *installer* da Microsoft.

---

## 🕹 Como Rodar (Run) os Demos

Se tudo construir perfeitamente, não caia no engano de rodar a raiz! O framework gera os executáveis binários perfeitamente prontos para dist num diretório especial agrupado da engine sem DLLs perdidas. É so arrancar a Sandbox principal:

```shell
.\build\bin\DEBUG\Demo2.exe
```

**[ 🎮 Controlando a Demo 2 ]** 
* Use os botões **`W`** e **`S`** para transacionar a Matriz Orbital do Eixo de Inclinação X (Pitch).
* Use os botões **`A`** e **`D`** para orbitar os Graus de Movimentação do Eixo Lateral Y (Yaw).
* Aperte com violência espaço: **`ESPAÇO`**. O Input acionará instantaneamente os ganchos da nossa Engine que assumem rotação em escala aleatória auto-guiada e ininterrupta da Matriz pro Shader sem input local. Batendo nela dinamicamente denovo transita para estado Parado e retoma suas ordens locais!
* **`ESCAPE / ESC`** finalizará a Window e todas as Alocações massivas da Engine serão derrubadas via Graceful-Exit seguro antes do SO desalinhar.
