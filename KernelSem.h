// File: KernelSem.h
#ifndef _KernelSem_h_
#define _KernelSem_h_

#include "PCB.h"

class Semaphore;

class KernelSem
{
private:
	volatile int val;
	Semaphore *mySem;
	friend class Semaphore;
	
	friend class Queue 
	{
	private:
		struct Elem							// ELEMENT LISTE:
		{
			PCB *pcb;                     	// - sadržani PCB,
			Elem* sled;                    	// - pokazivac na sledeci element,
			Elem (PCB *p)     				// - konstruktor.
			{ 
				pcb = p; 
				sled = 0; 
			}
		};
		Elem *prvi, *posl;              	// Pokazivac na pocetak i kraj liste.
		Queue& dodaj(PCB *p) 				// Dodavanje PCB-a.
		{
			posl = (!prvi ? prvi : posl->sled) = new Elem(*p);
			return *this;
		}
		PCB *izvadi()
		{
			PCB *i=prvi.pcb;
			prvi = prvi->sled;
			return i;
		}
	public:                                 // Konstruktori:
		Queue() { prvi = posl = 0; } 		// - podrazumevani,
		~Queue();                           // Destruktor.
	};
	Queue blocked;
	
	void block();
	void deblock();
};

#endif