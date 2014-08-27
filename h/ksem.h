// File: ksem.h
#ifndef _ksem_h_
#define _ksem_h_

#include <_null.h>

class Semaphore;
class PCB;

class KernelSem
{
private:
	volatile int val;
	Semaphore *mySem;
	friend class Semaphore;
	
	KernelSem(Semaphore *semaphore, int init);
	
	class Queue 
	{
	private:
		struct Elem					// ELEMENT LISTE
		{
			PCB *pcb;				// - PCB
			Elem* sled;
			Elem (PCB *p)
			{ 
				pcb = p; 
				sled = 0; 
			}
		};
		Elem *prvi, *posl;
	public:		
		unsigned dodaj(PCB *p) 		// Dodavanje PCB-a
		{
			Elem *novi = new Elem(p);
			if (novi != NULL)
			{
				posl = (!prvi ? prvi : posl->sled) = novi;
				return 1;
			}
			else return 0;
		}
		PCB *izvadi()
		{
			PCB *i=prvi->pcb;
			prvi = prvi->sled;
			return i;
		}	
		
		Queue() { prvi = posl = 0; } 
		~Queue();
	};

	Queue blocked;
	
	void block();
	void deblock();
};

#endif