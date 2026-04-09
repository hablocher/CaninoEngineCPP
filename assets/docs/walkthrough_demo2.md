# A Ascensão Tridimensional: Canino Engine 3D

Um marco absoluto na evolução desta arquitetura. A engine rompeu do simples desenho ortográfico de polígonos 2D lisos e ingressou de cabeça no poder bélico bruto dos ambientes Tridimensionais Fotorealistas Nativos usando a API Nativa do Windows sem nenhuma biblioteca em OOP (Objetos C++) pesada escondida de nós.

## Arquiteturas Fundamentadas

### O Módulo Matemático DOD (A API `Mat4` e Intrísecos SIMD)
Adicionamos ao core matemático purista as estruturas 3D autênticas (`math_ops.h`). A nova e brilhante struct `Mat4` de 64 bytes está sendo calculada em CPU com laços puristas não-alocados. Agora temos `Mat4RotateX`, `Mat4Translate`, `Mat4Perspective` e `Mat4Multiply` orquestrando o Model-View-Projection estritamente no formato *Data-Oriented*.

### O Leitor Binário de Texturas C-Style
Adotamos agressivamente o leitor unitário de Imagens Clássico da Industria `stb_image.h`, puxando ele solto no diretório `render` para que nenhuma DLL ou .Lib fantasma (Como LibJPEG etc) poluisse a compilação cruzada. Ele foi enclausurado estritamente dento do `canino::RenderCommand::CreateTexture()` para blindá-lo e prevenir vazamentos de dados pela engine principal. Nosso RHI retorna para o C++ apenas um `void*` (um ID3D11ShaderResourceView em Disfarce). 

### Texturas, Z-Buffer e Cubos na Placa Gráfica (DX11)
Desmontamos a máquina 2D do update anterior e forjamos uma Máquina 3D completa.
1. **DepthStencilView**: Implantamos e mapeamos Z-Buffer (24-bits profundidade + 8-bits stencil) para obliterar pixels obstruidos, impedindo que as faces ocultas do cubo reescrevam a tela.
2. **HLSL UV Sampler**: Injetamos as sintaxes de `Texture2D` e `SamplerState` no string de código das shaders na *Runtime*.
3. **Novo VBO Tridimensional**: Adicionamos 36 Vértices densos desenhando os Triângulos Espaciais e suas coordenadas texturizadas UV (Mapeando do 0.0 ao 1.0 em formato de envoltório em torno do Mesh X, Y, Z).

## Demo 2: Rotacionando a Criação
Isolamos essa feature avassaladora num target de CMake dedicado (`Demo2`).
O Demo invoca o Gateway `CreateTexture("assets/canino.jpg")` sem saber que tipo de bruxaria nativa ocorre abaixo, inicializa uma câmera movida para trás (`Mat4Translate(0, 0, 2)`) e captura input ao vivo. 
- Apertar `W / S` soma flutuações e ativa a matrix bruta `Mat4RotateX`.
- Apertar `A / D` dispara o `Mat4RotateY`.
- Apertar `Espaço` comuta de forma idempotente a adição flutuante massiva do giratório automático, mostrando o motor matemático enviando, todos os frames, 64-bytes escalares pro CBO nativo da GPU para renderização de um asset Texturizado AAA de Alta Definição.

O Pipeline de Renderização AAA purista se inicia hoje!
