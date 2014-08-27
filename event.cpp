#include "event.h"
#include "KERNEL.h"
#include "KernelEv.h"
//#include "IVTEntry.h"

Event::Event(IVTNo ivtNo)
{
	lock
	myImpl = new KernelEv(this, ivtNo);
	unlock
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