#include "event.h"
#include "KernelEv.h"
//#include "IVTEntry.h"

Event::Event(IVTNo ivtNo)
{
	lock
	myImpl = new KernelEv();
	myImpl->myEvent = this;
	myImpl->ivtNumber = ivtNo;
	myImpl->ulaz = IVTEntry::getIVTEntry(ivtNo);
	unlock
}

Event::~Event()
{
	/*	TODO	*/
}

void Event::wait()
{
	myImpl->wait();	
}

void Event::signal()
{
	myImpl->signal();	
}