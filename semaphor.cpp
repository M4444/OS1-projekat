#include "semaphor.h"
#include "KERNEL.h"

//virtual ~Semaphore ();	TODO

virtual Semaphore::Semaphore (int init=1)
{
	lock
	myImpl = new KernelSem();	//	prosledjivanje argumenata kostruktoru moze da poveca vreme zakljucavanja
	unlock
	myImpl->mySem = this;
	myImpl->mySem.val = init;
}

virtual void wait ()
{
	lockTake
	if(--myImpl.val<0)
	{
		myImpl.block();
	}
	unlockTake
}

virtual void signal()
{
	lockTake
	if(myImpl.val++<0)
	{
		myImpl.deblock();
	}
	unlockTake
}

int Semaphore::val()
{	
	return myImpl.val;
}