// File: KernelEv.h
#ifndef _KernelEv_h_
#define _KernelEv_h_

#include "event.h"
#include "IVTEntry.h"

class KernelEv
{	
private:
	friend class Event;
	IVTNo ivtNumber;
	IVTEntry *ulaz;
	Event *myEvent;
	
	void wait();
	void signal();	
};

#endif