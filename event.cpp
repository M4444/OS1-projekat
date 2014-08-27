#include "event.h"
#include "KERNEL.h"
#include "KernelEv.h"
#include <iostream.h>
//#include "IVTEntry.h"

Event::Event(IVTNo ivtNo)
{
	lock
	myImpl = new KernelEv(this, ivtNo);
	unlock
	if (myImpl == NULL)
	{
		lockTake
		cout<<"***Alokacija DOGADJAJA nije uspela!"<<endl;
		unlockTake
	}
}

Event::~Event()
{
	delete myImpl;
}

void Event::wait()
{
	myImpl->wait();	
}

void Event::signal()
{
	myImpl->signal();	
}