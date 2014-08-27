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
	
	KernelSem(Semaphore *semaphore, int init);
	
	class Queue 
	{
	private:
		struct Elem					// ELEMENT LISTE
		{
			PCB *pcb;				// - sadržani PCB
			Elem* sled;
			Elem (PCB *p)
			{ 
				pcb = p; 
				sled = 0; 
			}
		};
		Elem *prvi, *posl;
	public:		
		Queue& dodaj(PCB *p) 		// Dodavanje PCB-a
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
		
		Queue() { prvi = posl = 0; } 
		~Queue();
	};

	Queue blocked;
	
	void block();
	void deblock();
};

#endif