#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include <schedule.h>
#include <dos.h>

#define MAX_STACK_SIZE 4096	//	64KB/16B = 4096


//virtual ~Thread();		TODO
//waitToComplete();			TODO

void Thread::waitToComplete()
{
	/*	TODO	*/
}

void Thread::start()
{
	Scheduler::put(myPCB);
}

Thread::~Thread()
{
	/*	TODO	*/
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	// Kreiranje procesa
	lock
	myPCB = new PCB();
	myPCB->myThread = this;
	if (stackSize>MAX_STACK_SIZE) stackSize = MAX_STACK_SIZE;	/*	!!!!	takodje proveriti da li je stek premali, a i na drugim mestima */
	unsigned* stek = new unsigned[stackSize];

	stek[stackSize-1] = 0x200;	//	setovan I fleg u pocetnom PSW-u za nit
	stek[stackSize-2] = FP_SEG(PCB::wrapper);
	stek[stackSize-3] = FP_OFF(PCB::wrapper);

	myPCB->sp = FP_OFF(stek+stackSize-12); 	//	svi sacuvani registri pri ulasku u interrupt rutinu
	myPCB->ss = FP_SEG(stek+stackSize-12);
	myPCB->bp = FP_OFF(stek+stackSize-12);
	myPCB->zavrsio = 0;
	if (timeSlice != 0)
	{
		myPCB->kvant = timeSlice;
		myPCB->neogranicen = 0;
		
	}
	else 
	{
		myPCB->kvant = 0;
		myPCB->neogranicen = 1;
	}
	unlock
}	

void dispatch()	 // sinhrona promena konteksta 
{
	asm cli;
	zahtevana_promena_konteksta = 1;
	timer();
	asm sti;
}