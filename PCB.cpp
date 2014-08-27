#include "PCB.h"
#include "dos.h"
#include "thread.h"
#include "Semaphore.h"
//#include "KERNEL.h"

#define MAX_STACK_SIZE 4096	//	64KB/16B = 4096

volatile PCB *PCB::running=0;

void PCB::wrapper()
{
	PCB::running->myThread->run();
	// kad zavrsi...
	PCB::running->zavrsio = 1;
	while (PCB::running->waitThread->val()<0) PCB::running->waitThread->signal(); 
	// signaliziranje svim nitima koje cekaju na semaforu niti koja je sad zavrsila
	dispatch(); 
}

PCB::PCB(Thread *thread, StackSize stackSize, Time timeSlice)
{
	myThread = thread;
	
	if (stackSize>MAX_STACK_SIZE) stackSize = MAX_STACK_SIZE;
	//if (stackSize<12000) stackSize = 128;
	stack = new unsigned[stackSize];

	stack[stackSize-1] = 0x200;			//	setovan I fleg u pocetnom PSW-u za nit
	stack[stackSize-2] = FP_SEG(PCB::wrapper);
	stack[stackSize-3] = FP_OFF(PCB::wrapper);

	sp = FP_OFF(stack+stackSize-12); 	//	svi sacuvani registri pri ulasku u interrupt rutinu
	ss = FP_SEG(stack+stackSize-12);
	bp = FP_OFF(stack+stackSize-12);
	zavrsio = 0;
	blokiran = 0;
	if (timeSlice != 0)
	{
		kvant = timeSlice;
		neogranicen = 0;
	}
	else
	{
		kvant = 0;
		neogranicen = 1;
	}
	waitThread = new Semaphore(0);
}

PCB::~PCB()
{
	delete waitThread;
	delete [] stack;
}