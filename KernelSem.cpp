#include "KernelSem.h"

void KernelSem::suspend(PCB *pcb)
{
	Queue::dodaj(pcb);
}

PCB* KernelSem::getWaiting()
{
	return Queue::izvadi();
}

KernelSem::Queue::~Queue()  			// Destruktor.
{           
	while (prvi) 
	{	 
		Elem* stari = prvi; prvi = prvi->sled; delete stari;
	}
}