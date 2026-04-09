# Design: The Sandbox Demo Integral (ECS + Jobs + RHI)

O apoteose físico matemático.

## Resoluções de Design (Aprovadas)
1. **The Native Graphic Backend**: Vamos sujar as mãos apenas o mínimo com a API legada contida internamente no Windows (OpenGL Immediate Mode Nativo) apenas para que a RHI saia do breu e apresente dados tridimensionais limpos sem precisarmos re-escrever parsers de shaders complexos de milhares de linhas desta vez.

## Mudanças do Sandbox
1. A RHI Front-End (`rhi.h`) agora dita a Regra: `RenderCommand::DrawQuad()`.
2. A main da Sandbox forjará *2.000* Inimigos (Entidades) colados nas arenas. Eles possuirão componentes `Transform` (Position e Velocidade) e um componente `Sprite` (RGB).
3. Todo mísero frame submeterá 2.000 jobs físicos individuais para as 16 threads lógicas processarem os quiques das colisões isoladamente na Matemática Pura DOD contra as bordas -1.0 e 1.0 NDC do GL.
4. Após o Culling e processamento Físico, a Rendering Thread puxa os dados e preenche a Primitiva da RHI com centenas de sub-calls O(1).
