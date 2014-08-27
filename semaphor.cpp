#include "semaphor.h"
#include "ksem.h"
#include "thread.h"
#include "KERNEL.h"
#include <iostream.h>

Semaphore::Semaphore(int init)
{
	lock
	myImpl = new KernelSem(this, init);
	unlock
	if (myImpl == NULL)
	{
		lockTake
		cout<<"***Alokacija SEMAFORA nije uspela!"<<endl;
		unlockTake
	}
}

Semaphore::~Semaphore()
{
	delete myImpl;
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