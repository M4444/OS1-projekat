#include "IVTEntry.h"
#include "semaphor.h"
#include <iostream.h>
#include "KERNEL.h" 
#include "dos.h"

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt novaRutina)
{
	if (nizUlaza[ivtNo]==NULL || ivtNo==8)
	{
		sem = new Semaphore(0);
		if (sem != NULL)
		{
			BrUlaza = ivtNo;
			newRoutine = novaRutina;
			nizUlaza[BrUlaza] = this;
			
			lock
			oldRoutine = getvect(BrUlaza);
			setvect(BrUlaza, newRoutine);
			setvect(BrUlaza+60, oldRoutine);
			unlock
		}
		else 
		{
			lockTake
			cout<<"***Alokacija IVT ULAZA nije uspela!"<<endl;
			unlockTake
		}
	}
	else 
	{
		lockTake
		cout<<"***Ulaz "<<ivtNo<<" je zauzet!"<<endl;
		unlockTake
	}
}

IVTEntry::~IVTEntry()
{
	nizUlaza[BrUlaza] = NULL;
	delete sem;
	
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
	sem->wait();
}

void IVTEntry::signal()
{
	sem->signal();
	zahtevana_promena_konteksta = 1;
}

void IVTEntry::CallOldRoutine()
{
	geninterrupt(BrUlaza);
}

IVTEntry *IVTEntry::nizUlaza[256] = {NULL};