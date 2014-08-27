#include "KernelEv.h"
#include "IVTEntry.h"
#include <_null.h>

KernelEv::KernelEv(Event *event, IVTNo ivtNo)
{
	myEvent = event;
	ivtNumber = ivtNo;
	ulaz = IVTEntry::getIVTEntry(ivtNo);
}

void KernelEv::wait()
{
	if (IVTEntry::nizUlaza[ivtNumber]!=NULL) ulaz->wait();
}

void KernelEv::signal()
{
	ulaz->signal();
}