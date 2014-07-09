#include "PCB.h"
#include "thread.h"

volatile PCB *PCB::running=0;

void PCB::wrapper()
{
	PCB::running->myThread->run();
	// kad zavrsi...
	PCB::running->zavrsio = 1;
	while (PCB::running->waitThread.val()<0) PCB::running->waitThread.signal(); 
	// signaliziranje svim nitima koje cekaju na semaforu niti koja je sad zavrsila
	dispatch(); 
}