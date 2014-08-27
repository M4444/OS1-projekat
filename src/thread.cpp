#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include <schedule.h>
#include <iostream.h>
#include "Semaphore.h"

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
	this->waitToComplete();
	delete myPCB;
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	lock
	myPCB = new PCB(this, stackSize, timeSlice);
	unlock
	if (myPCB == NULL)
	{
		lockTake
		cout<<"***Alokacija NITI nije uspela!"<<endl;
		unlockTake
	}
}	

void dispatch()	 // sinhrona promena konteksta 
{
	lock
	zahtevana_promena_konteksta = 1;
	timer();
	unlock
}