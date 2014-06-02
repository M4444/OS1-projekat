#include <iostream.h>
#include <dos.h>
#include <schedule.h>

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

// Zabrana ispisa
#define lockCout lockFlag=0;
// Dozvola ispisa
#define unlockCout 	lockFlag=1;\
					if (zahtevana_promena_konteksta) dispatch();

#define MAX_STACK_SIZE 4096	//	64KB/16B = 4096

class Thread {
public:

	void start()
	{
		Scheduler::put(myPCB);
	}
	//void waitToComplete(); 	TODO
	//virtual ~Thread();		TODO
	
protected:
	friend class PCB;
	
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	
	virtual void run() {}
	
private:
	PCB* myPCB;
};

Thread::Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice)
	{
		// Kreiranje procesa
		lock
		myPCB = new PCB();
		if (stackSize>MAX_STACK_SIZE) stackSize = MAX_STACK_SIZE;
		unsigned* stek = new unsigned[stackSize];

		stek[stackSize-1] = 0x200;	//	setovan I fleg u pocetnom PSW-u za nit
		stek[stackSize-2] = FP_SEG(PCB::wrapper);
		stek[stackSize-3] = FP_OFF(PCB::wrapper);

		myPCB->sp = FP_OFF(stek+stackSize-12); 	//	svi sacuvani registri pri ulasku u interrupt rutinu
		myPCB->ss = FP_SEG(stek+stackSize-12);
		myPCB->bp = FP_OFF(stek+stackSize-12);
		myPCB->zavrsio = 0;
		myPCB->kvant = timeSlice;
		unlock
	}	

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
	// kad zavrsi...
	running->zavrsio = 1;
	dispatch(); 
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

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;
			
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
			
			if (! PCB::running->zavrsio) Scheduler::put((PCB *) PCB::running);
			PCB::running=Scheduler::get();		// Scheduler
	  
			tsp = PCB::running->sp;
			tss = PCB::running->ss; 
			tbp = PCB::running->bp;

			brojac = PCB::running->kvant;

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

public Slova : Thread
{
public:
	Slova(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(1024,40){}
private:
	static ID tekID=0;
	ID id = ++tekID;
	virtual run()
	{
		for (int i =0; i < 30; ++i)
		{
			lockCout
			cout<<"id = "<id<" i = "<<i<<endl;
			unlockCout
			
			for (int k = 0; k<10000; ++k)
				for (int j = 0; j <30000; ++j);
		}
		//exitThread();
	}	
};

Slova a,b,m;
void doSomething()
{
	lock
	a = Slova(1024,40);
	cout<<"napravio a"<<endl;
	a->start();
	
	b = Slova(1024,20)
	cout<<"napravio b"<<endl;
	b->start();

	m = Slova(1024,20)
	cout<<"napravio b"<<endl;
	PCB::running = m->PCB;
	unlock

	for (int i = 0; i < 15; ++i) 
	{
		lock
		cout<<"main "<<i<<endl;
		unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	cout<<"Happy End"<<endl;
}

int main()
{
	inic();
	
	doSomething();
	
	restore();
 
	return 0;
}

