#include "ksem.h"
#include "thread.h"
#include "PCB.h"
#include <schedule.h>
#include "KERNEL.h"

void KernelSem::block()
{
	this->blocked.dodaj((PCB *) PCB::running);
	PCB::running->blokiran = 1;
	dispatch();
}

void KernelSem::deblock()
{
	PCB *izvadjen=this->blocked.izvadi();
	izvadjen->blokiran = 0;
	Scheduler::put(izvadjen);
}

KernelSem::Queue::~Queue()  			// Destruktor.
{           
	while (prvi) 
	{	 
		Elem* stari = prvi; 
		prvi = prvi->sled; 
		delete stari;
	}
}