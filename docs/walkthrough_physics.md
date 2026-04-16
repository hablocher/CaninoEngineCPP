# Motor Híbrido de Colisão 3D (AABB-Sliding)

Implementamos um solver minimalista e ultrarrápido para barreiras espaciais! O núcleo da arquitetura funciona inteiramente em $O(1)$ detectando transições em eixos tridimensionais (X, Y e Z) e resolvendo a interseção expulsando a malha do jogador pro lado de fora seguindo regras algébricas que recriam a física dos Anos 90.

## 1. Engine Core API (`canino/physics`)
Criamos duas structs essenciais no escopo `canino::physics`:
* `AABB`: Axis-Aligned Bounding Box. Uma caixa cúbica não rotacionável universal capaz de enjaular as massas dos objetos. 
* `CollisionHit`: Retorna se o objeto tocou, e o mais importante, o subvetor **Minimum Translation Vector (MTV)**.

### Minimum Translation Vector (MTV)
> [!NOTE]
> Quando andamos e o nosso jogador "entra" acidentalmente numa estátua ou pedra pela frente, nós não paramos o frame instantaneamente. O motor calcula qual dos 3 eixos penetrou _MENOS_ na pedra e nos devolve um `Vetor de Expulsão` na direção exata oposta. Como resultado, o jogador acaba sendo "arrastado" pelas arestas das colunas. Esse é o segredo matemático do famoso _Wall Sliding_ de jogos como Quake e Doom.

## 2. Abordagem FPS (Gameplay Controller)
O `FPSDemo.exe` agora opera de forma orgânica (abandonamos os vetores arbitrários mágicos e alturas forçadas):

1. **Geração do Mundo**: No boot, criamos uma enxurrada de `AABB` a partir do `TransformComponent` dos pilares texturizados e os enfileiramos num Array Linear.
2. **Posição Virtual Contínua**: Toda tecla WASD pressionada não move mais a câmera de cara. Movimenta uma malha temporária chamada _"proposedEye"_.
3. **Escaneamento de Atrito**: Avaliamos a malha de teste contra cada um dos pilares e no chão no Sweep Cycle. Caso toque uma quina, a câmera resvala para fora através do `MTV`.
4. **Resolução de Gravidade**: Se o `MTV` empurrou você pra Cima (+Y), o código compreende matematicamente que está tocando um chão genérico e zera a velocidade de queda! O mesmo vale se debater com um teto. Nenhuma altura Y (como `2.0f`) foi hardcodada.

### Testar 🎮
Você já pode testar o sistema abrindo a build executável limpa:
```powershell
.\build\bin\DEBUG\FPSDemo.exe
```
