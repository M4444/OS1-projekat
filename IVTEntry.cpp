#include "IVTEntry.h"

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt novaRutina)
{
	BrUlaza = ivtNo;
	newRoutine = novaRutina;
	sem = Semaphore(0);
	nizUlaza[BrUlaza] = this;
	
	lock
	oldRoutine = getvect(BrUlaza);
	setvect(BrUlaza, newRoutine);
	setvect(BrUlaza+60, oldRoutine);
	unlock

}

IVTEntry::~IVTEntry()
{
	nizUlaza[BrUlaza] = 0;
	
	lock
	setvect(BrUlaza, oldRoutine);
	unlock
}

IVTEntry *IVTEntry::getIVTEntry(IVTNo num)
{
	return nizUlaza[num];
}

void IVTEntry::wait()
{
	sem.wait();
}

void IVTEntry::signal()
{
	sem.signal();
}

void IVTEntry::CallOldRoutine()
{
	geninterrupt(BrUlaza);
}

IVTEntry *IVTEntry::nizUlaza[256] = {0};	/*	!!!!	proveriti da li moze {NULL}	*/