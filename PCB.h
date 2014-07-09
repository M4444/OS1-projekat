// File: PCB.h
#ifndef _PCB_h_
#define _PCB_h_

class Thread;
//class Semaphore;

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
	
	//Semaphore *sem;
	
	static volatile PCB *running; 
	static void wrapper();
	
	friend void interrupt timer();	
};

#endif