// File: PCB.h
#ifndef _PCB_h_
#define _PCB_h_

#include "Semaphore.h"

class Thread;

class PCB
{
private:
	Thread *myThread;
	friend class Thread;
	friend class KernelSem;
	
	unsigned sp;
	unsigned ss;
	unsigned bp;
	
	unsigned zavrsio;
	unsigned kvant;
	unsigned neogranicen; 
	unsigned blokiran;
	
	Semaphore waitThread;
	
	static volatile PCB *running; 
	static void wrapper();
	
	friend void interrupt timer();
};

#endif