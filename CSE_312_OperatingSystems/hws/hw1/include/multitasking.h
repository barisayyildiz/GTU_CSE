 
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{
    
    struct CPUState
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;

        /*
        common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds;
        */
        common::uint32_t error;

        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;
        common::uint32_t esp;
        common::uint32_t ss;  

        CPUState(const CPUState& other);

    } __attribute__((packed));
    
    
    class Task
    {
    friend class TaskManager;
    private:
        common::uint8_t stack[4096]; // 4 KiB
        CPUState* cpustate;
        common::uint32_t isTerminated;  // true, if terminated
    public:
        Task(GlobalDescriptorTable *gdt, void entrypoint());
        ~Task();
        Task(const Task& other);
        common::uint32_t getIsTerminated();
        void setIsTerminated();
    };
    
    
    class TaskManager
    {
    private:
        Task* tasks[256];
        int numTasks;
        int numReadyTasks;  // number of process that is not terminated yet
        int currentTask;
    public:
        TaskManager();
        ~TaskManager();
        void TerminateCurrentProcess();
        int getNumTasks(){return numTasks;}
        bool CloneTask();
        bool AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);
    };
    
    
    
}


#endif