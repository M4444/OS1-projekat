// File: KERNEL.h
#ifndef _KERNEL_h_
#define _KERNEL_h_

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

// Zabrana ispisa
#define lockCout lockFlag=0;
// Dozvola ispisa
#define unlockCout 	lockFlag=1;\
					if (zahtevana_promena_konteksta) dispatch();
					
extern volatile int zahtevana_promena_konteksta;
extern volatile unsigned lockFlag;

void interrupt timer();
void inic();
void restore();

void tick();	//	poziva se u funkciji timer() da bi korisnik bio svestan protoka vremena

#endif