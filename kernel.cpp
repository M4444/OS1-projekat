#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "PCB.h"
#include <schedule.h>
#include "KERNEL.h"

volatile unsigned lockFlag = 1;	//	fleg za zabranu promene konteksta
// Pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int brojac;
volatile int zahtevana_promena_konteksta = 0;

void interrupt timer()	//	prekidna rutina
{
	if (!zahtevana_promena_konteksta)
	{	
		if(!PCB::running->neogranicen) brojac--;
		tick();
	}		
	if (brojac == 0 && !PCB::running->neogranicen || zahtevana_promena_konteksta) 
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
			lockTake
			cout << "Promena konteksta!  Brojac = " << brojac << endl; 
				//	ako neko vec vrsi ispis, lockFlag je vec na 0 
				//	i zato se nece ni poceti promena konteksta, pa samim tim
				//	se ne moze desiti ni ovaj ispis (ne moze se desiti paralelan
				//	ispis iz neke od niti i prekidne rutine)
			lock //	u nekim slucajevima se desi da se prekidi omoguce unutar cout<<... 
			lockFlag=1;
			// kraj ispisa
			
			if ((!PCB::running->zavrsio) && (!PCB::running->blokiran)) Scheduler::put((PCB *) PCB::running);
			PCB::running = Scheduler::get();		// Scheduler
	  
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
		                                              
	//zahtevana_promena_konteksta = 0;
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

/*
typedef void interrupt (*pInterrupt)(...);
pInterrupt oldRoutine;
//void interrupt timer(...);

// postavlja novu prekidnu rutinu
void inic2()
{
	lock
	asm cli		
	oldRoutine = getvect(0x8);	// 	pamti staru rutinu
	setvect(0x8, timer);		//	postavlja novu rutinu
	setvect(0x8+0x60, oldRoutine);	//	postavlja staru rutinu na int 60
	asm sti
	unlock
}

// vraca staru prekidnu rutinu
void restore2()
{
	lock
	asm cli	
	setvect(0x8, oldRoutine);
	asm sti
	unlock
	
}*/

class userMainThread : public Thread
{
public:
	userMainThread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice, int argc, char* argv[]):Thread(stackSize,timeSlice)
	{ 
		Uargc = argc; 
		Uargv = argv;
	}
	int uMreturn;
private:
	int Uargc;
	char **Uargv;
	void run()
	{
		uMreturn = userMain(Uargc, Uargv);
	}
};

int main(int argc, char	**argv)
{
	inic();
	lock
	userMainThread *uM = new userMainThread(1024, 20, argc, argv);
	cout<<"napravio userMain"<<endl;
	uM->start();
	dispatch();
	unlock
	lock
	cout<<"Poslati argumenti: ";
	for (int ii=1; ii<argc; ii++)
	{
		cout<<argv[ii]<<" ";
	}
	cout<<endl;
	unlock
	for (int i=0; i<7; ++i) 
	{
		lock
		cout<<"main "<<i<<endl;
		unlock

		for (int j=0; j<30000; ++j)
			for (int k=0; k<30000; ++k);
	}
	cout << "Main Happy End" << endl;
	restore();
 
	return uM->uMreturn;
}