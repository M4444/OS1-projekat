#include "semaphor.h"
#include "ksem.h"
#include "thread.h"
#include "KERNEL.h"

//virtual ~Semaphore ();	TODO

Semaphore::Semaphore(int init)
{
	lock
	myImpl = new KernelSem();	//	prosledjivanje argumenata kostruktoru moze da poveca vreme zakljucavanja
	myImpl->mySem = this;
	myImpl->val = init;
	unlock
}

Semaphore::~Semaphore()
{
	/*	TODO	*/
}

void Semaphore::wait()
{
	lockTake
	if(--(myImpl->val)<0)
	{
		myImpl->block();
		dispatch();
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