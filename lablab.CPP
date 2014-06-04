//pretpostavljeni memorijski model: huge

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

class PCB
{
public:
	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned zavrsio;
	int kvant;
};

typedef void interrupt (*pInterrupt)(...);

//	globalne promenljive
PCB *p[3];
volatile PCB* running; 
volatile unsigned lockFlag = 1;	//fleg za zabranu promene konteksta

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int brojac = 55;
volatile int zahtevana_promena_konteksta = 0;

void interrupt timer(...)	// prekidna rutina
{
	int dummy=0;

	if (!zahtevana_promena_konteksta) brojac--; 
	if (brojac == 0 || zahtevana_promena_konteksta) 
	{
		if (lockFlag)
		{
			zahtevana_promena_konteksta = 0; 
				//ako je eventualno i bila zahtevana 
				//promena konteksta, sada ce se obaviti pa treba obrisati 
				//fleg za zahtev
		
			asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;
			
			//ispis unutar prekidne rutine
			lockCout
			cout<< "Promena konteksta!  Brojac= " << brojac << endl; 
				//ako neko vec vrsi ispis, lockFlag je vec na 0 
				//i zato se nece ni poceti promena konteksta, pa samim tim
				//se ne moze desiti ni ovaj ispis (ne moze se desiti paralelan
				//ispis iz neke od niti i prekidne rutine)
			asm cli; // u nekim slucajevima se desi da se prekidi omoguce unutar cout<<... 
			lockFlag=1;
			//kraj ispisa
			
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

void dispatch()	 // sinhrona promena konteksta 
{
	asm cli;
	zahtevana_promena_konteksta = 1;
	timer();
	asm sti;
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina
pInterrupt oldRoutine;

// postavlja novu prekidnu rutinu
void OLDinic()
{
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax
	
		mov ax, word ptr es:0022h 	//; pamti staru rutinu
		mov word ptr oldTimerSEG, ax	
		mov ax, word ptr es:0020h	
		mov word ptr oldTimerOFF, ax	

		mov word ptr es:0022h, seg timer	//postavlja 
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 		//	postavlja staru rutinu	
		mov word ptr es:0182h, ax 	//  na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax
		
		pop ax
		pop es
		sti
	}
}

// postavlja novu prekidnu rutinu
void inic()
{
	asm	cli
			
	oldRoutine = getvect(0x8);	// 	pamti staru rutinu
	setvect(0x8, timer);			//	postavlja novu rutinu
	setvect(0x60, oldRoutine);	//	postavlja staru rutinu na int 60
	
	asm	sti
}

// vraca staru prekidnu rutinu
void OLDrestore()
{
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		
		sti
	}
}

void restore()
{
	asm cli
	
	setvect(0x8, oldRoutine);
		
	asm	sti
	
}

void exitThread()
{
	running->zavrsio = 1;
	dispatch();
}  

void a()
{
	for (int i =0; i < 30; ++i)
	{
		//begin lock
		lockCout
		//end lock
		
		cout<<"u a() i = "<<i<<endl;
		
		//begin unlock
		lockFlag=1;		//ovakav unlock je potreban da bi se proverilo da li 
						//je zatrazeno preuzimanje u toku ispisa;
		if (zahtevana_promena_konteksta) dispatch(); //ako jeste onda se sada vrsi eksplicitno preuzimanje;
		//end unlock
			//voditi racuna da je ovo samo pomocno resenje za obezbjedjenje kriticne sekcije koje 
			//radi na jednoprocesorskim sistemima; 
						//iako su semafori jedno od univerzalnih resenje, ovakav pristup moze da bude bolji pri 
						//resavanju domaceg ukoliko se ispis koristi za debagovanje ...
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}

void b(){
	for (int i =0; i < 30; ++i) 
	{
		//begin lock
		lockCout
		//end lock
		
		cout<<"u b() i = "<<i<<endl;
		
		//begin unlock
		lockFlag=1;		//ovakav unlock je potreban da bi se proverilo da li 
						//je zatrazeno preuzimanje u toku ispisa;
		if (zahtevana_promena_konteksta) dispatch(); //ako jeste onda se sada vrsi eksplicitno preuzimanje;
		//end unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}


void createProcess(PCB *newPCB, void (*body)())
{
	unsigned* st1 = new unsigned[1024];

	st1[1023] = 0x200;	//	setovan I fleg u
						// 	pocetnom PSW-u za nit
	st1[1022] = FP_SEG(body);
	st1[1021] = FP_OFF(body);

	newPCB->sp = FP_OFF(st1+1012); 	//svi sacuvani registri 
									//pri ulasku u interrupt 
									//rutinu
	newPCB->ss = FP_SEG(st1+1012);
	newPCB->bp = FP_OFF(st1+1012);
	newPCB->zavrsio = 0;
}



void doSomething()
{
	lock
	p[1] = new PCB();
	createProcess(p[1],a);
	cout<<"napravio a"<<endl;
	p[1]->kvant = 40;
	Scheduler::put(p[1]);
	
	p[2] = new PCB();
	createProcess(p[2],b);
	cout<<"napravio b"<<endl;
	p[2]->kvant = 20;
	Scheduler::put(p[2]);

	p[0] = new PCB();
	p[0]->kvant = 20;
	p[0]->zavrsio = 0;

	running = p[0];
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

