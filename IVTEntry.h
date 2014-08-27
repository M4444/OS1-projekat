// File: IVTEntry.h
#ifndef _IVTEntry_h_
#define _IVTEntry_h_

//#include "event.h"
typedef unsigned char IVTNo;

typedef void interrupt (*pInterrupt)(...);

// Priprema ulaz (PREPAREENTRY)
#define PREPAREENTRY(IVTNum, CallOld)\
void interrupt PrekidnaRutina##IVTNum(...)\
{\
	IVTEntry::nizUlaza[IVTNum]->signal();\
	if(CallOld == 1) IVTEntry::nizUlaza[IVTNum]->CallOldRoutine();\
}\
IVTEntry Entry##IVTNum = IVTEntry(IVTNum, PrekidnaRutina##IVTNum);

class Semaphore;

class IVTEntry
{
public:
	IVTEntry(IVTNo ivtNo, pInterrupt novaRutina);
	~IVTEntry();
	
	static IVTEntry *nizUlaza[256];
	
	static IVTEntry *getIVTEntry(IVTNo num);
	
	void wait();
	void signal();
	void CallOldRoutine();
private:
	IVTNo BrUlaza;
	
	Semaphore *sem;
	pInterrupt oldRoutine, newRoutine;
};

#endif