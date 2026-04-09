# Design: The Data-Oriented Job System

Uma engine DOD genuína enche 100% dos núcleos da máquina do Jogador.

## Resoluções de Design (Aprovadas)
1. **The Help-Execute Trick**: Acordado e Sancionado. Nossa *Main Thread* abandonará qualquer espera ociosa de `sleep(ms)`. Ela e todas as `Work Threads` atacarão furiosamente o RingBuffer matando tarefas enquanto a barreira de contexto atômico for maior do que zero.
2. **Zero Allocation**: Um buffer Circular estático pré-moldado engessará o limite do frame, impedindo OS Memory-Gaps e travamentos de sistema. Se a fila de submissão lotar, **quem estiver submetendo é obrigado a processar os Jobs na hora (Yield Stealing)**.

## Arquitetura DOD Threading
1. Headers isolados no sub-diretório Core: `job_system.h` e arquivo .cpp.
2. Inicialização cega de `std::thread::hardware_concurrency() - 1` na subida da engine (Platform Layer).
3. Sem `std::function`. Apenas Function Pointers C-Style (`void(*)(void*)`) amarrados na `struct Job`.

## Verification Plan
O simulador do CTest enviará **500.000 (meio milhão)** de Jobs C-Style pedindo incremento atômico e adição de strings sob contenção de contexto cego para provar estritamente que as falhas de *Race-Condition* C++ desapareceram e O Ring-Buffer escala em performance absoluta sem vazar memória limitante.
