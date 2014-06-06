#include "semaphor.h"
#include "KERNEL.h"

//virtual ~Semaphore ();	TODO

		//virtual void wait ();
		//virtual void signal();

virtual Semaphore::Semaphore (int init=1)
{
	lock
	myImpl = new KernelSem();
	unlock
	myImpl->mySem = this;
	myImpl->mySem.val = init;
}

virtual void wait ()
{
	val--;		/*	ne moze direktno	*/
	if(val<0)
	{
		myImpl.suspend(PCB::running);
		/*	prebaci kontekst na myImpl.getWaiting();	*/
	}
}

virtual void signal()
{
	val++;		/*	atomicno (?)	*/
	if(val<=0)
	{
		Scheduler::put(myImpl.getWaiting());
	}
}

int Semaphore::val()
{	
	return myImpl.val;
}