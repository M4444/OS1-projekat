// File: PCB.h
#ifndef _PCB_h_
#define _PCB_h_

#include "thread.h"

class Semaphore;

class PCB
{
private:
	Thread *myThread;
	friend class Thread;
	friend class KernelSem;
	
	unsigned *stack;
	
	unsigned sp;
	unsigned ss;
	unsigned bp;
	
	unsigned zavrsio;
	unsigned kvant;
	unsigned neogranicen; 
	unsigned blokiran;
	
	Semaphore *waitThread;
	
	PCB(Thread *thread, StackSize stackSize, Time timeSlice);
	~PCB();
	
	static volatile PCB *running; 
	static void wrapper();
	
	friend void interrupt timer();
};

#endif