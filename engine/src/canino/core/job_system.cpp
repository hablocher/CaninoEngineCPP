#include <canino/core/job_system.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace canino {

// ============================================
// O COFRE DATA-ORIENTED (PIMPL C-Style Oculto)
// ============================================
struct Job {
    JobFunction EntryPoint;
    void* Data;
    JobContext* Ctx; // Quem engatilhou
};

// Array Estático Fixado em RAM. Sem malloc() ou std::queue dinâmico no meio do runtime.
constexpr uint32_t MAX_QUEUE_SIZE = 4096;
static Job g_JobQueue[MAX_QUEUE_SIZE];
static uint32_t g_QueueHead = 0;
static uint32_t g_QueueTail = 0;

static std::mutex g_QueueMutex;
static std::condition_variable g_WakeCondition;

// Trabalhadores Brutos do Silicio
static std::vector<std::thread> g_WorkerThreads;
static bool g_IsSystemRunning = false;

// ============================================
// THE HELP-EXECUTE ENGINE
// ============================================
// Rotina de consumo que rouba um item caso exista, processa, e abate o contador atômico do Contexto.
// Qualquer linha da Engine (seja WorkerThread, ou a Main Thread esperando fisíca) pode dar Pop!
static bool ExecuteNextJobSilently() {
    Job job;
    {
        std::lock_guard<std::mutex> lock(g_QueueMutex);
        if (g_QueueHead == g_QueueTail) {
            return false; // Vazão de queue seca, nada pra eu roubar
        }
        job = g_JobQueue[g_QueueHead];
        g_QueueHead = (g_QueueHead + 1) % MAX_QUEUE_SIZE; // Push C-Style Array Pointer
    }
    
    // Execução Violenta Assíncrona no C-Pointer
    job.EntryPoint(job.Data);

    // Despacho final O(1) com Memory_Release pro Kernel Cache Coherence
    if (job.Ctx) {
        job.Ctx->Counter.fetch_sub(1, std::memory_order_release);
    }
    
    return true;
}

// O Loop infinito da classe servil
static void NativeWorkerThreadLoop() {
    while (g_IsSystemRunning) {
        // Tento processar uma carga de tarefas até o talo
        if (!ExecuteNextJobSilently()) {
            // Se estou ocioso mesmo, fecho os olhos pra OS não arder os núcleos em SpinLock
            std::unique_lock<std::mutex> lock(g_QueueMutex);
            g_WakeCondition.wait(lock, [] { 
                return (g_QueueHead != g_QueueTail) || !g_IsSystemRunning; 
            });
        }
    }
}

// ============================================
// FACADE EXPOSED API
// ============================================
void JobSystem_Initialize() {
    g_QueueHead = 0;
    g_QueueTail = 0;
    g_IsSystemRunning = true;

    // Engorda a quantidade de Workers basedando-se nos Núcleos Físicos e Lógicos subtraindo
    // a MainThread Master (nós já estamos usando 1).
    uint32_t numCores = std::thread::hardware_concurrency();
    uint32_t numWorkers = numCores > 1 ? numCores - 1 : 1; 

    for (uint32_t i = 0; i < numWorkers; i++) {
        g_WorkerThreads.emplace_back(NativeWorkerThreadLoop);
    }
}

void JobSystem_Shutdown() {
    g_IsSystemRunning = false;
    g_WakeCondition.notify_all(); // Acorda soldados para perceber a morte

    for (std::thread& worker : g_WorkerThreads) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    g_WorkerThreads.clear();
}

void JobSystem_Dispatch(JobContext* ctx, JobFunction function, void* data) {
    if (ctx) {
        ctx->Counter.fetch_add(1, std::memory_order_relaxed);
    }

    while (true) {
        bool bPushedSuccess = false;
        {
            std::lock_guard<std::mutex> lock(g_QueueMutex);
            uint32_t nextTail = (g_QueueTail + 1) % MAX_QUEUE_SIZE;
            if (nextTail != g_QueueHead) {
                // Existe espaço fisico na Cache Circular DOD
                g_JobQueue[g_QueueTail].EntryPoint = function;
                g_JobQueue[g_QueueTail].Data = data;
                g_JobQueue[g_QueueTail].Ctx = ctx;
                g_QueueTail = nextTail;
                bPushedSuccess = true;
            }
        }

        if (bPushedSuccess) {
            // Empurrou o dado pra esteira. Acorda um escravo da OS pra processar.
            g_WakeCondition.notify_one();
            break;
        } else {
            // SITUAÇÃO APOCALIPTICA DE BACKPRESSURE: A Queue encheu antes dos workers limparem!
            // Main Thread que está injetando DEVE processar junto pra liberar espaço inves de esperar morta.
            ExecuteNextJobSilently();
        }
    }
}

void JobSystem_Wait(JobContext* ctx) {
    if (!ctx) return;
    
    // O pulo do gato supremo (Wait-Stealing). 
    // Enquanto tivermos pelo menos 1 micro-tarefa presa lá rodando em escravos, 
    // ou solta na queue pra esse grupo de contexto: NÓS AJUDAMOS. Ninguém dorme.
    while (ctx->Counter.load(std::memory_order_acquire) > 0) {
        if (!ExecuteNextJobSilently()) {
            // A queue local tá zerada, mas o Counter>0. Significa que os Pointers estão ativamente 
            // no CacheL1 dos Escravos executando no último momento. Não há o que a gente ajudar rodando jobs,
            // então perco do Kernel Contextual via OS scheduler pra evitar 100% de queimação inutil de CPU 
            std::this_thread::yield(); 
        }
    }
}

}
