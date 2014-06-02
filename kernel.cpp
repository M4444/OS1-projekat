#include <iostream.h>
#include <dos.h>
#include <schedule.h>

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

#define MAX_STACK_SIZE 4096	//	64KB/16B = 4096

class Thread {
public:

	//void start(); 			TODO
	//void waitToComplete(); 	TODO
	//virtual ~Thread();		TODO
	
protected:
	friend class PCB;
	
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice)
	{
		// Kreiranje procesa
			lock
			myPCB = new PCB();			/*	!!!!	oprez sa operatorom new - nije thread-safe	*/
			if (stackSize>MAX_STACK_SIZE) stackSize = MAX_STACK_SIZE;
			unsigned* stek = new unsigned[stackSize];

			stek[1023] = 0x200;	//	setovan I fleg u pocetnom PSW-u za nit
			stek[1022] = FP_SEG(PCB::wrapper);
			stek[1021] = FP_OFF(PCB::wrapper);

			myPCB->sp = FP_OFF(stek+1012); 	//	svi sacuvani registri pri ulasku u interrupt rutinu
			myPCB->ss = FP_SEG(stek+1012);
			myPCB->bp = FP_OFF(stek+1012);
			myPCB->zavrsio = 0;
			myPCB->kvant = timeSlice;
		//	//
		cout << "Naprazvljena je nova nit." << endl;		/*	!!!!	*/
		Scheduler::put(myPCB);
	}	
	
	virtual void run() {}
	
private:
	PCB* myPCB;
};

void dispatch()	 // sinhrona promena konteksta 
{
	asm cli;
	zahtevana_promena_konteksta = 1;
	timer();
	asm sti;
}

class PCB
{
private:
	Thread myThread;
	
	unsigned sp;
	unsigned ss;
	unsigned bp;
	
	unsigned zavrsio;
	unsigned kvant;
	
	volatile static PCB* running; 
	static void wrapper();
};

void PCB::wrapper()
{
	PCB::running->myThread->run();
	//...
	//	PCB::running->zavrsio = 1;
	//	...
}

volatile unsigned lockFlag = 1;	//	fleg za zabranu promene konteksta
// Pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int brojac = 55;
volatile int zahtevana_promena_konteksta = 0;

void interrupt timer()	//	prekidna rutina
{
	if (!zahtevana_promena_konteksta) brojac--; 
	if (brojac == 0 || zahtevana_promena_konteksta) 
	{
		if (lockFlag)
		{
			zahtevana_promena_konteksta = 0; 
				//	ako je eventualno i bila zahtevana 
				//	promena konteksta, sada ce se obaviti pa treba obrisati 
				//	fleg za zahtev
		
			asm {
				//	cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;
			
			// ispis unutar prekidne rutine
			lockCout
			cout<< "Promena konteksta!  Brojac= " << brojac << endl; 
				//	ako neko vec vrsi ispis, lockFlag je vec na 0 
				//	i zato se nece ni poceti promena konteksta, pa samim tim
				//	se ne moze desiti ni ovaj ispis (ne moze se desiti paralelan
				//	ispis iz neke od niti i prekidne rutine)
			asm cli; //	u nekim slucajevima se desi da se prekidi omoguce unutar cout<<... 
			lockFlag=1;
			// kraj ispisa
			
			if (! running->zavrsio) Scheduler::put((PCB *) running);
			running=Scheduler::get();		// Scheduler
	  
			tsp = running->sp;
			tss = running->ss; 
			tbp = running->bp;

			brojac = running->kvant;

			asm {
				mov sp, tsp   // restore sp
				mov ss, tss
				mov bp, tbp
			}     
		}
		else zahtevana_promena_konteksta = 1;
	} 
    
	// poziv stare prekidne rutine koja se 
    // nalazila na 08h, a sad je na 60h
    // poziva se samo kada nije zahtevana promena
    // konteksta – tako se da se stara
    // rutina poziva samo kada je stvarno doslo do prekida	
	if(!zahtevana_promena_konteksta) asm int 60h;
		                                              
	zahtevana_promena_konteksta = 0;
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina
// postavlja novu prekidnu rutinu
void inic()
{
	asm{
		cli
		push es
		push ax

		mov ax,0   //	inicijalizuje rutinu za tajmer
		mov es,ax
	
		mov ax, word ptr es:0022h 	//	pamti staru rutinu
		mov word ptr oldTimerSEG, ax	
		mov ax, word ptr es:0020h	
		mov word ptr oldTimerOFF, ax	

		mov word ptr es:0022h, seg timer	//	postavlja 
		mov word ptr es:0020h, offset timer //	novu rutinu

		mov ax, oldTimerSEG	 		//	postavlja staru rutinu	
		mov word ptr es:0182h, ax 	//  na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax
		
		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore()
{
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG	//	vraca star
		mov word ptr es:0022h, ax		//	prekidnu rutinu
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		
		sti
	}
}

int main()
{
	inic();
	
	doSomething();
	
	restore();
 
	return 0;
}

