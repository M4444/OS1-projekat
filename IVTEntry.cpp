#include "IVTEntry.h"

IVTEntry::IVTEntry(IVTNo ivtNo)
{
	BrUlaza = ivtNo;
	sem = Semaphore(0);
	nizUlaza[ivtNo] = this;
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

void IVTEntry::setOld(pInterrupt old)
{
	oldRoutine = old;
}

pInterrupt IVTEntry::getOld()
{
	return oldRoutine;
}

IVTEntry *IVTEntry::nizUlaza[256] = {0};