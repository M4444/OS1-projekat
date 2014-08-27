#include "semaphor.h"
#include "ksem.h"
#include "thread.h"
#include "KERNEL.h"
//#include <iostream.h>

Semaphore::Semaphore(int init)
{
	lock
	myImpl = new KernelSem(this, init);
	unlock
}

Semaphore::~Semaphore()
{
	delete myImpl;		
	// resen problem brisanja pomocu pakazivaca na semafore
	// tako da se na stvaraju lokalni objekti koji potom pozivanju destruktore
	// ne zna se tacno koji je razlog bio za to
}

void Semaphore::wait()
{
	lockTake
	if(--(myImpl->val)<0)
	{
		myImpl->block();
	}
	unlockTake
}

void Semaphore::signal()
{
	lockTake
	if((myImpl->val++)<0)
	{
		myImpl->deblock();
	}
	unlockTake
}

int Semaphore::val() const
{	
	return myImpl->val;
}