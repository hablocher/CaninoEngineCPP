# Design: O The Entity Component System (ECS Core)

A Era da "Orientação a Objetos" e heranças gigantes desabou. Um Inimigo na Engine não é nada além da Entidade Nº 42, um índice inteiro amarrando puramente Arrays colossais na memória.

## Resoluções de Design (Aprovadas)
1. **Zero RTTI & Compile-Time Hashing**: Nada de `typeid()`. Como a engine é linkada como DLL e quebra limites de memória entre Sandbox e Core, identificadores de componentes numéricos gerariam conflitos severos. Introduziremos **Compile-Time String Hashing (CTE)** usando o identificador de assinatura unívoco do MSVC (`__FUNCSIG__`). Isso garante que todo componente é uma chave hash única e estrita universal resolvida na Compilação, O(0) em Runtime.
2. **The Sparse Set**: Cada classe da Engine será dividida em um array fixo de bytes que preenche os blocos seqüencialmente (O(1) iterations / L1 Cache friendly) e um array leve vazio interligando os indíces de trás para frente.

## Arquitetura DOD ECS
1. Headers isolados no sub-diretório `canino/ecs/`.
    * `ecs.h` (Definições cruas de Entity Type, e Constant Expression Hash).
    * `component_array.h` (A engrenagem do *Sparse Set*, o Swap-and-Pop invisível C-Style).
    * `registry.h` (Gerenciador generalista e Spawner Universal de Entidades com Free-List Queue de reaproveitamento de mortes).

## Verification Plan
O simulador do CTest enviará criações e destruições massivas de Entidades fragmentadas. O código vai iterar checando as mutações no SparseSet verificando se as Deleções forçaram fisicamente as Array Contíguas preencherem seus vácuos perfeitamente usando as magias do Component Array.
