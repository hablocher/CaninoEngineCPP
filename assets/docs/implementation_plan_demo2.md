# Design: Demo 2 (Textured 3D Cube interativo)

A Renderização Tridimensional Nativa. O Abandono Oficial do plano esférico bidimensional.

## Resoluções de Design (Aprovadas)
Você acatou os sacrifícios arquiteturais com firmeza. A Canino Engine mergulhará de cabeça nas fundações DirectX/OpenGL para viabilizar Câmeras, Coordenadas Téxteis, Z-Buffering, Oclusão e Álgebra Linear Estrita.

## Etapas Executadas Simultaneamente
1. **Injeção do Stb_Image (Sub-Sistema Textura)**: Iremos puxar a biblioteca c-style monolítica direto pro núcleo via PowerShell para evitar a falência do pipeline C++.
2. **Matrizes Mat4 (Math SIMD)**: Para fazer o Axis girar sem bibliotecas bloatadas O.O., escreveremos equações algébricas locais para projetar Quaternions e ViewMatrix na Unidade Lógica Aritmética pura.
3. **Depth Engine DX11**: Configurando o V-Buffer de profundidade e instanciando 24 vértices do cubo fotorealista e seus mapeamentos UV para envelopar a textura.
4. **O Aplicativo Demo 2**: O módulo autônomo receberá sua janela contendo Input Assíncrono W,A,S,D e Espaco (Ativando Rotações Automáticas no JobSystem) integrados diretamente contra os CBOs recém aprimorados do DirectX11.
