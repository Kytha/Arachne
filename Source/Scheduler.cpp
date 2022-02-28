#pragma once
#include "Scheduler.h"
#include <assert.h>
#include <chrono>
#include "Console.h"
#include <algorithm>
#define NOMINMAX
#include <windows.h>

Scheduler* Scheduler::s_Instance = nullptr;

Scheduler::Scheduler()
{
    print("Detecting hardware threads...\n");
    const int hardwareThreads = std::thread::hardware_concurrency();
    if(hardwareThreads <= 0)
        assert("Unable to get hardware concurrency \n");
    print("Hardware threads Detected: ", hardwareThreads, "\n");
    m_NumOfWorkers = std::min(MAX_WORKERS, hardwareThreads);
    m_Workers.resize(m_NumOfWorkers);


    m_Fibers[0].prev = nullptr;
    m_Fibers[0].next = &m_Fibers[1];
    m_Fibers[MAX_FIBERS-1].prev = &m_Fibers[MAX_FIBERS-2];
    m_Fibers[MAX_FIBERS-1].next = nullptr;
    for(int i = 1; i < MAX_FIBERS-1; i++) {
        m_Fibers[i].next = &m_Fibers[i + 1];
        m_Fibers[i].prev = &m_Fibers[i - 1];
    }
    for(int i = 0; i < MAX_FIBERS; i++)
    {
        m_Fibers[i].id = i + 1;
    }
    m_fiberHead = m_Fibers;
}

Fiber* Scheduler::AllocateFiber()
{
    Scheduler& self = GetScheduler();
    if(!self.m_fiberHead)
        return nullptr;
    Fiber* fiber = self.m_fiberHead;
    if(fiber->prev)
        fiber->prev->next = fiber->next;
    if(fiber->next)
        fiber->next->prev = fiber->prev;
    self.m_fiberHead = self.m_fiberHead->next;
    return fiber;
}

void Scheduler::FreeFiber(Fiber* fiber)
{
    Scheduler& self = GetScheduler();
    fiber->next = self.m_fiberHead;
    fiber->prev = nullptr;
    if(self.m_fiberHead)
        self.m_fiberHead->prev = fiber;
    self.m_fiberHead = fiber;
}

Scheduler::~Scheduler()
{
    for(uint16_t i = 0; i < m_NumOfWorkers; i++)
    {
        print("Deleting worker ", i, "\n");
        delete m_Workers[i];
    }
}

Scheduler& Scheduler::GetScheduler()
{
    if(s_Instance != nullptr)
        return *s_Instance; 
    s_Instance = new Scheduler();
    return *s_Instance;  
}

static void Work(int id)
{
    while(true) {
        //struct Fiber *fiber = worker_wait_task_or_quiesce();
        //if(!fiber)
            //return;
        //assert(fiber);
        //sched_task_run(fiber);
    }
}

bool Scheduler::Start()
{
    // TO DO : Check if already started
    Scheduler& self = GetScheduler();
    for(uint16_t i = 0; i < self.m_NumOfWorkers; i++)
    {
        Worker* worker = new Worker();
        worker->id = i;

        print("Spinning up worker thread #", worker->id, "...\n");
        worker->thread = std::thread([worker]
        {

            while(!worker->shouldStop)
            {
                print("Running thread #", worker->id, ": on CPU ", GetCurrentProcessorNumber(), "\n");
                Scheduler& scheduler = Scheduler::GetScheduler();
                std::unique_lock<std::mutex> lock(scheduler.m_ReadyMutex);
                if(scheduler.m_FiberReadyQueue.empty()) {
                    print("No fibers to process, worker #", worker->id ," is going to sleep \n");
                    scheduler.m_ReadyCondition.wait(lock, [&] {return !scheduler.m_FiberReadyQueue.empty() || worker->shouldStop;});
                }
                if(worker->shouldStop)
                    break;
                Fiber* fiber = scheduler.m_FiberReadyQueue.front();
                scheduler.m_FiberReadyQueue.pop();
                lock.unlock();

                print("Worker #", worker->id ,": Assigned fiber #", fiber->id, "\n");
                //worker_do_work(id);
                //worker_notify_done(id);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                print("Worker #", worker->id ,": Finished processing fiber #", fiber->id, "\n");
                
                const std::lock_guard<std::mutex> requestLock(scheduler.m_RequestMutex);
                print("Worker #", worker->id ,": Freeing fiber #", fiber->id, "\n");
                scheduler.FreeFiber(fiber);
                // TO DO: Worker
            }
            print("Spinning down worker thread #", worker->id, "...\n");
            // TO DO: Clean Up
        });
        DWORD_PTR dw = SetThreadAffinityMask(worker->thread.native_handle(), DWORD_PTR(1) << i);
        if (dw == 0)
        {
            DWORD dwErr = GetLastError();
            std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
        }
        self.m_Workers[i] = worker;
    }
    return true;   
}

bool Scheduler::Stop()
{
    Scheduler& self = GetScheduler();
    for(uint16_t i = 0; i < self.m_NumOfWorkers; i++)
    {
       self.m_Workers[i]->shouldStop = true;
    }
    self.m_ReadyCondition.notify_all();
    WaitForWorkers();
    delete s_Instance;
    s_Instance = nullptr;
    return true;
}

void Scheduler::WaitForWorkers()
{
    Scheduler& self = GetScheduler();
    for(uint16_t i = 0; i < self.m_NumOfWorkers; i++)
    {
        self.m_Workers[i]->thread.join();
    }
}

void Scheduler::InitializeContext(Fiber* fiber, void* task)
{
    Byte* stackEnd = (Byte*)fiber->stackMemory;
    Byte* stackBase = stackEnd + STACK_SIZE;
    stackBase = (Byte*)ALIGNED((uintptr_t)stackBase, 32);
    if(stackBase >= stackEnd + STACK_SIZE)
        stackBase -= 32;

    stackBase -= 8;
    *(uint64_t*)stackBase = (uintptr_t)exitTaskCode;

    stackBase -= 8;
    *(uint64_t*)stackBase = (uintptr_t)task;

    memset(&fiber->context, 0, sizeof(fiber->context));
    fiber->context.rsp = (uintptr_t)stackBase;

    fiber->context.fpucw = (((uint16_t)(0x0  & 0x1 )) << 12)
                | (((uint16_t)(0x0  & 0x3 )) << 10)
                | (((uint16_t)(0x2  & 0x3 )) << 8 )
                | (((uint16_t)(0x0  & 0x1 )) << 7 )
                | (((uint16_t)(0x3f & 0x3f)) << 0 );

    fiber->context.mxcsr = (((uint32_t)(0x0  & 0x1 )) << 15)
                | (((uint32_t)(0x0  & 0x3 )) << 13)
                | (((uint32_t)(0x3f & 0x3f)) << 7 )
                | (((uint32_t)(0x0  & 0x1 )) << 6 );
}

uint16_t Scheduler::CreateTask(taskFunction task, void *arg, uint32_t flags, int priority)
{
    Scheduler& self = GetScheduler();
    const std::lock_guard<std::mutex> lock(self.m_RequestMutex);
    Fiber* fiber = self.AllocateFiber();
    if(!fiber)
        assert("To many fibers");

    fiber->priority = priority;
    fiber->flags = flags;
    fiber->arg = arg;
    fiber->returnValue = 0;
    fiber->stackMemory = self.m_Stacks[fiber->id - 1];
    self.InitializeContext(fiber,task);
    self.Reactivate(fiber);
    return fiber->id;
}

void Scheduler::Reactivate(Fiber* fiber)
{
    Scheduler& self = GetScheduler();
    std::unique_lock<std::mutex> lock(self.m_ReadyMutex);
    self.m_FiberReadyQueue.push(fiber);
    lock.unlock();
    self.m_ReadyCondition.notify_one();
}
