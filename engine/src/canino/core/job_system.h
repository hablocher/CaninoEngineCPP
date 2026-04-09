#pragma once
#include <canino/core/core.h>
#include <atomic>
#include <cstdint>

namespace canino {

    // Contexto Abstrato atômico que o Engine-Master usará para "Esperar" as tasks secundárias finalizarem.
    // É o passe livre p/ sincronizar Física, Renderização e Transformações Múltiplas.
    struct JobContext {
        std::atomic<uint32_t> Counter{0};
    };

    // Assinatura de hardware crua sem lixo de `std::function` (V-Table overhead).
    // Passamos o jobIndex para o Data saber quem ele é caso seja um Job de Array de Componentes O(1).
    typedef void (*JobFunction)(void* data);

    CANINO_API void JobSystem_Initialize();
    CANINO_API void JobSystem_Shutdown();

    // The Help-Execute Injector.
    // Envia um trabalho cego pro array C-style de RingBuffer protegido.
    // Se a RingQueue estiver cheia, essa própria thread se transforma em worker pra aliviar processamento!
    CANINO_API void JobSystem_Dispatch(JobContext* ctx, JobFunction function, void* data);

    // The Sleep-Killer Wait.
    // A Test-Thread ou Render-Thread principal que precisa desta barreira ATACARÁ os cálculos pendentes
    // até seu contexto ser consumido, impedindo osciosidade térmica do processador.
    CANINO_API void JobSystem_Wait(JobContext* ctx);
}
