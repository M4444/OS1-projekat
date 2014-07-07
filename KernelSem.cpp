#include "KernelSem.h"

void KernelSem::block()
{
	this.blocked.dodaj(PCB::running);
	takeCont(izvadi());
}

void KernelSem::deblock()
{
	Scheduler::put(this.blocked.izvadi());
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