// File: IVTEntry.h
#ifndef _IVTEntry_h_
#define _IVTEntry_h_

#include "event.h"
#include "KERNEL.h"
#include "semaphor.h"

typedef void interrupt (*pInterrupt)(...);

//	prekidna rutina
#define PrekidnaRutina(IVTNo, CallOld)\
void interrupt PrekidnaRutina##IVTNo(...)\
{\
	Entry##IVTNo.signal();\
	if(CallOld == 1) asm int OldIVTNo;\
}
// postavlja novu prekidnu rutinu
#define inicE(IVTNo) inic##IVTNo();

#define PostavljanjePR(IVTNo)\
void inic##IVTNo()\
{\
	lock\
	Entry##IVTNo.setOld(getvect(0x##IVTNo))\
	setvect(0x##IVTNo, PrekidnaRutina##IVTNo);\
	setvect(0x##IVTNo+0x60, Entry##IVTNo.getOld());\
	unlock\
}
// vraca staru prekidnu rutinu
#define restoreE(IVTNo) restore##IVTNo();

#define VracanjePR(IVTNo)\
void restore##IVTNo()\
{\
	lock\
	setvect(0x##IVTNo, Entry##IVTNo.getOld());\
	unlock\
}
// Priprema ulaz
#define PREPAREENTRY(IVTNo, CallOld)\
class IVTEntry##IVTNo : IVTEntry\
{\
	IVTEntry##IVTNo(IVTNo ivtNo):IVTEntry(ivtNo)\
	{\
		lock\
		Entry##IVTNo.setOld(getvect(0x##IVTNo))\
		setvect(0x##IVTNo, PrekidnaRutina##IVTNo);\
		setvect(0x##IVTNo+0x60, Entry##IVTNo.getOld());\
		unlock\
	}\
	\
	~IVTEntry##IVTNo(IVTNo ivtNo)\
	{\
		lock\
		setvect(0x##IVTNo, Entry##IVTNo.getOld());\
		unlock\
	}\
}\
IVTEntry##IVTNo Entry##IVTNo = IVTEntry(IVTNo);\
\
PrekidnaRutina(IVTNo, CallOld)

class IVTEntry
{
public:
	IVTEntry(IVTNo ivtNo);
	
	static IVTEntry *getIVTEntry(IVTNo num);
	
	void wait();
	void signal();
	void setOld(pInterrupt old);
	pInterrupt getOld();
private:
	IVTNo BrUlaza;
	static IVTEntry *nizUlaza[256];
	
	Semaphore sem;
	pInterrupt oldRoutine;
};

#endif