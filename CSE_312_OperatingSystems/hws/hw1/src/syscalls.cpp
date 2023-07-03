
#include <syscalls.h>
 
using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;
 
SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber, TaskManager* taskManager, GlobalDescriptorTable* gdt)
:    InterruptHandler(interruptManager, InterruptNumber  + interruptManager->HardwareInterruptOffset())
{
    this->taskManager=taskManager;
    this->gdt = gdt;
}

SyscallHandler::~SyscallHandler()
{
}


void printf(char*);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;
    

    switch(cpu->eax)
    {
        case 2:
            // handles fork syscall
            printf("\n,fork system call...");
            this->taskManager->CloneTask();
            break;
        case 4:
            printf((char*)cpu->ebx);
            break;
            
        default:
            break;
    }

    
    return esp;
}

