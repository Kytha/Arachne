#pragma once
#include "Fiber.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <array>
#include <queue>

#define MAX_FIBERS (4096)
#define STACK_SIZE (16 * 1024)
#define LARGE_STACK_SIZE (4 * 1024 * 1024)
#define MAX_WORKERS 16

#define ALIGNED(val, align)     (((val) + ((align) - 1)) & ~((align) - 1))

using Byte = char;

struct Worker {
    std::thread thread;
    std::atomic<bool> shouldStop = false;
    std::atomic<bool> isIdle = true;
    Context context;
    uint16_t id;
    
};

class Scheduler
{
public:
    Scheduler();
    virtual ~Scheduler();
    static bool Start();
    static bool Stop();
    static void WaitForWorkers();
    static Scheduler& GetScheduler(); 
    void InitializeContext(Fiber* fiber, void* task);
    static uint16_t CreateTask(taskFunction task, void *arg, uint32_t flags = FiberFlags::NONE, int priority = 0);
    Fiber* AllocateFiber();
    void FreeFiber(Fiber* fiber);
    void Reactivate(Fiber* fiber);
private:
    std::vector<Worker*> m_Workers;
    Fiber m_Fibers[MAX_FIBERS];
    Byte m_Stacks[MAX_FIBERS][STACK_SIZE];
    uint16_t m_NumOfWorkers;
    std::mutex m_ReadyMutex;
    std::mutex m_RequestMutex;
    std::condition_variable m_ReadyCondition;
    std::queue<Fiber*> m_FiberReadyQueue;
    static Scheduler* s_Instance;
    Fiber * m_fiberHead;
};