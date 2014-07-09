#include "ksem.h"
#include "thread.h"
#include "PCB.h"
#include <schedule.h>

void KernelSem::block()
{
	this->blocked.dodaj((PCB *) PCB::running);
	//takeContext(izvadi());
	dispatch();
}

void KernelSem::deblock()
{
	Scheduler::put(this->blocked.izvadi());
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