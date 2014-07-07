#include "PCB.h"
#include "thread.h"

volatile PCB *PCB::running=0;

void PCB::wrapper()
{
	PCB::running->myThread->run();
	// kad zavrsi...
	PCB::running->zavrsio = 1;
	dispatch(); 
}