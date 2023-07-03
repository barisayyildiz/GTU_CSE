
#include <multitasking.h>

using namespace myos;
using namespace myos::common;

void printf(char *str);
void printfHex32(uint32_t);

// copy constructor for CPUState
CPUState::CPUState(const CPUState& other)
{
    eax = other.eax;
    ebx = other.ebx;
    ecx = other.ecx;
    edx = other.edx;

    esi = other.esi;
    edi = other.edi;
    ebp = other.ebp;

    error = other.error;

    eip = other.eip;
    cs = other.cs;
    eflags = other.eflags;
    esp = other.esp;
    ss = other.ss;
}


Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    this->isTerminated = 0;

    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    /*
    cpustate -> gs = 0;
    cpustate -> fs = 0;
    cpustate -> es = 0;
    cpustate -> ds = 0;
    */
    
    // cpustate -> error = 0;    
   
    // cpustate -> esp = ;
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
    
}

Task::~Task()
{
}

// copy constructor for Task
Task::Task(const Task& other)
{
    // printf(", copy constructor for task\n");
    // Allocate new stack
    for (int i = 0; i < sizeof(stack); i++) {
        stack[i] = other.stack[i];
    }

    // Allocate new CPU state
    // printf("oldcpustate : ");
    // printfHex32((uint32_t)other.cpustate);
    // printf("oldcpustate.eip : ");
    // printfHex32((uint32_t)other.cpustate->eip);
    // printf("\n");

    cpustate = new CPUState(*other.cpustate);
    // printf("newcpustate : ");
    // printfHex32((uint32_t)cpustate);
    // printf("newcpustate.eip : ");
    // printfHex32((uint32_t)cpustate->eip);
    // printf("\n");

    isTerminated = 0;
}

uint32_t Task::getIsTerminated(){
    return this->isTerminated;
}

void Task::setIsTerminated(){
    this->isTerminated = 1;
}

        
TaskManager::TaskManager()
{
    numTasks = 0;
    numReadyTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}

// sets isTerminated flag as true and reduces the number of ready tasks by one
void TaskManager::TerminateCurrentProcess(){
    if(numReadyTasks <= 0){
        return;
    }
    tasks[currentTask]->setIsTerminated();
    numReadyTasks--;
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    tasks[numTasks++] = task;
    numReadyTasks++;
    return true;
}

// it used to handle fork system call
// add the currently running process to process table
bool TaskManager::CloneTask()
{
    if(numTasks + 1 >= 256){
        return false;
    }
    // Task* childTask = new Task(*tasks[currentTask]);
    // this->AddTask(childTask);
    this->AddTask(tasks[currentTask]);
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numReadyTasks <= 0)
        return cpustate;
    
    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;

    // printf("\noldcpustate : ");
    // printfHex32((uint32_t)tasks[currentTask]->cpustate);
    // printf(", oldcpustate.eip : ");
    // printfHex32((uint32_t)tasks[currentTask]->cpustate->eip);
    // printf("\n");

    // enters in a while loop and searches a non terminated process with Round-Robin algorithm
    while(1){
        currentTask++;
        currentTask %= numTasks;
        if(!tasks[currentTask]->getIsTerminated()){
            // printf("\ncurrentTask : ");
            // printfHex32(currentTask);
            // printf(", ");


            // printf("\nnewcpustate : ");
            // printfHex32((uint32_t)tasks[currentTask]->cpustate);
            // printf(", newcpustate.eip : ");
            // printfHex32((uint32_t)tasks[currentTask]->cpustate->eip);
            // printf("\n");

            return tasks[currentTask]->cpustate;
        }
    }
}
