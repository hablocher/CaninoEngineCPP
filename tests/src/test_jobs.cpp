#include "test_core.h"
#include <canino/core/job_system.h>
#include <atomic>
#include <iostream>

// Struct de teste enviada pela Array Data-Oriented cega
struct StressTestPayload {
    std::atomic<uint64_t>* MasterSum;
};

// Funcao C purista alocada pros Pointers das Trabalhadoras
void HeavySimulationJob(void* data) {
    StressTestPayload* payload = (StressTestPayload*)data;
    if (payload && payload->MasterSum) {
        // Disputa mortal de Cache-Line e Sync hardware test
        payload->MasterSum->fetch_add(1, std::memory_order_relaxed);
    }
}

static bool Test_MassiveThreadDispatch() {
    canino::JobSystem_Initialize();

    std::atomic<uint64_t> resultSum{0};
    StressTestPayload payload = { &resultSum };

    canino::JobContext battleContext;
    
    uint32_t totalJobs = 500000; // Meio Milhao de Disparos Asincronos!!!

    for (uint32_t i = 0; i < totalJobs; ++i) {
        canino::JobSystem_Dispatch(&battleContext, HeavySimulationJob, &payload);
    }

    // A prova do Help-Steal:
    // Nós colocamos 500 mil jobs na ram. A Queue local DOD MAX tem apenas 4096 espaćos.
    // Pra entrar 500 mil jobs lá, a MainThread foi violentamente obrigada a Invadir a Array circular vazada e matar jobs como Worker.
    canino::JobSystem_Wait(&battleContext); // Destroi o que faltou, sincroniza

    canino::JobSystem_Shutdown();

    // Checagem pós holocausto: Cada 1 do meio milhao foi processado sem engulir nem duplicar
    CANINO_EXPECT(resultSum.load() == totalJobs);

    return true;
}

bool RunThreadingTests() {
    CANINO_RUN_TEST(Test_MassiveThreadDispatch);
    return true;
}
