#include "KERNEL.h"
#include <iostream.h>
#include <schedule.h>
#include "thread.h"
#include <dos.h>
#include "PCB.h"

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
	if (brojac == 0 && !(PCB::running->neogranicen) || zahtevana_promena_konteksta) 
	{
		if (lockFlag)
		{
			zahtevana_promena_konteksta = 0; 
		
			asm {
				mov tsp, sp		//	cuva sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;
			
			if ((!PCB::running->zavrsio) && (!PCB::running->blokiran)) Scheduler::put((PCB *) PCB::running);
			PCB::running = Scheduler::get();	// Scheduler
	  
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
	
	if(!zahtevana_promena_konteksta) asm int 60h;
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

		mov ax, word ptr oldTimerSEG	//	vraca staru
		mov word ptr es:0022h, ax		//	prekidnu rutinu
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		
		sti
	}
}

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
	int MainReturn;
	inic();
	lock
	userMainThread *uM = new userMainThread(1024, 20, argc, argv);
	unlock
	if (uM == NULL) return -1;
	cout<<"userMain created"<<endl;
	uM->start();
	dispatch();
	cout << "Main Happy End" << endl;
	restore();
	MainReturn = uM->uMreturn;
	delete uM;
 
	return MainReturn;
}