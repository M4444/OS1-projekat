// File: KernelEv.h
#ifndef _KernelEv_h_
#define _KernelEv_h_

#include "event.h"

class Event;
class IVTEntry;

class KernelEv
{
private:
	KernelEv(Event *event, IVTNo ivtNo);

	friend class Event;
	IVTNo ivtNumber;
	IVTEntry *ulaz;
	Event *myEvent;
	
	void wait();
	void signal();	
};

#endif