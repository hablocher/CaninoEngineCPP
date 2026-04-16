# DirectX 11 Texture & Constant Buffer Layout Bug (Post-Mortem)

## O Problema Principal
Durante o desenvolvimento da pipeline de texturas da Canino Engine, blocos inteiros deveriam exibir texturas, mas ao invés disso, renderizavam os modelos geométricos com a cor preta absoluta ou simplesmente desapareciam na renderização (incorporando a exata cor do fundo).

Este bug foi causado puramente por uma **falha silenciosa de empacotamento na ABI entre Memória C++ e Shaders HLSL**.

### Causas Raízes
1. **D3D11_SUBRESOURCE_DATA Pitch Error:** Inicialmente, a alocação do D3D11 Texture Buffer ignorava o `SysMemSlicePitch`. Memória tridimensional para texturas 2D no DirectX costuma falhar implicitamente em drivers da arquitetura Windows. Isso foi resolvido passando `SysMemSlicePitch = width * height * canais`.
2. **Descarte e Padding de Constant Buffer (CBO):**
    A nossa engine injetava todos os dados em uma macro-estrutura única atrelada ao slot `register(b0)` do pipeline de shaders:
    ```cpp
    __declspec(align(16)) struct ConstantBufferData {
        int UseTexture[4];
        float SolidColor[4];
        Mat4 MVP;
    };
    ```
    Como o Vertex Shader da nossa arquitetura **apenas rotaciona e move matrizes** (usa apenas a matriz `MVP`), a reflexão em tempo real gerada pelo compilador de Shader (`D3DCompile`) assumia que o limite do buffer limitava-se a leitura das matrizes. Como recompensa, ele ignorava ou não preenchia corretamente offsets dos booleanos como `UseTexture` por truncagem. A variável `UseTexture.x` colapsava para zero, inviabilizando todas as flags de renderização e desabando num cubo estático de lixo de memória na GPU.

## A Solução (CBO Split e Alinhamento Estrutural)
Para quebrar a lógica instável de padding, adotamos a prática industrial _"Constant Buffer O-Splitting"_:

A struct gigantesca foi reescrita e quebrada em entidades isoladas em baias de transmissão separadas `(b0 e b1)` baseando-se no escopo do processador gráfico:
1. `CBOMatrices` (64 bytes) -> Previamente atrelado ao Vertex Shader com o pipeline puramente matricial de geometria.
2. `CBOMaterials` (32 bytes) -> Atrelado ao Pixel Shader para calcular Flags, Luzes, Texturas e Efeitos de Rasterização.

Limpando as baias de cada Constant Buffer iterativamente com `memset()`, acabamos com qualquer ruído de ponteiro deixado por chamadas do frame anterior, e cravamos o acerto estático de Bytes de GPU com blocos minúsculos impossíveis de sofrerem desaninhamento!

## Garantias e Unit Testing da Engine
Criamos também o artefato CTest (`tests/src/test_rhi.cpp`) com verificação algébrica do byte-sizeof da RHI do DirectX para que ela nunca quebre o alinhamento múltiplo estrito de 16-bytes da Microsoft.
