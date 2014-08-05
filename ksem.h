// File: ksem.h
#ifndef _ksem_h_
#define _ksem_h_

class Semaphore;
class PCB;

class KernelSem
{
private:
	volatile int val;
	Semaphore *mySem;
	friend class Semaphore;
	
	class Queue 
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
	public:  
		//KernelSem();			TODO
		Queue& dodaj(PCB *p) 				// Dodavanje PCB-a.
		{
			posl = (!prvi ? prvi : posl->sled) = new Elem(p);
			return *this;
		}
		PCB *izvadi()
		{
			PCB *i=prvi->pcb;
			prvi = prvi->sled;
			return i;
		}	
											// Konstruktori:
		Queue() { prvi = posl = 0; } 		// - podrazumevani,
		~Queue();                           // Destruktor.
	};

	Queue blocked;
	
	void block();
	void deblock();
};

#endif