#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include <schedule.h>
#include "Semaphore.h"
//#include <dos.h>
//#include <iostream.h>

void Thread::waitToComplete()
{
	myPCB->waitThread->wait();
}

void Thread::start()
{
	Scheduler::put(myPCB);
}

Thread::~Thread()
{
	this->waitToComplete();		/*	sta ako nit pozove sopstveni destruktor?	*/
	delete myPCB;
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	lock
	myPCB = new PCB(this, stackSize, timeSlice);
	unlock
}	

void dispatch()	 // sinhrona promena konteksta 
{
	lock
	zahtevana_promena_konteksta = 1;
	timer();
	unlock
}