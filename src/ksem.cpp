#include "ksem.h"
#include "PCB.h"
#include "thread.h"
#include "KERNEL.h"
#include <schedule.h>
#include <iostream.h>

KernelSem::KernelSem(Semaphore *semaphore, int init)
{
	mySem = semaphore;
	val = init;
}

void KernelSem::block()
{
	unsigned uspeh = blocked.dodaj((PCB *) PCB::running);
	if (uspeh != 0)
	{
		PCB::running->blokiran = 1;
	}
	else
	{
		lockTake
		cout<<"***Blokiranje na semaforu nije uspelo!"<<endl;
		unlockTake
	}
	dispatch();
}

void KernelSem::deblock()
{
	PCB *izvadjen=this->blocked.izvadi();
	izvadjen->blokiran = 0;
	Scheduler::put(izvadjen);
}

KernelSem::Queue::~Queue()
{           
	while (prvi) 
	{	 
		Elem* stari = prvi; 
		prvi = prvi->sled; 
		delete stari;
	}
}