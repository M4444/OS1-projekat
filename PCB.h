// File: PCB.h
#ifndef _PCB_h_
#define _PCB_h_

#include "thread.h"

class PCB
{
private:
	Thread *myThread;
	friend class Thread;
	
	unsigned sp;
	unsigned ss;
	unsigned bp;
	
	unsigned zavrsio;
	unsigned kvant;
	
	static volatile PCB *running; 
	static void wrapper();
	friend void interrupt timer();
	friend void doSomething();
};

#endif