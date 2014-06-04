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

volatile int brojac = 20;
volatile int zahtevana_promena_konteksta = 0;

void interrupt timer()	//	prekidna rutina
{
	if (!zahtevana_promena_konteksta)
	{	
		brojac--;
		tick();
	}		
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
			cout<< "Promena konteksta!  Brojac = " << brojac << endl; 
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

