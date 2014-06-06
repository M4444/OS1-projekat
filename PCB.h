// File: PCB.h
#ifndef _PCB_h_
#define _PCB_h_

class Thread;

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
	static void radi(Thread *);
	friend void interrupt timer();	
	friend void doSomething();		/*	!!!!	resiti na drugi nacin	*/
};

#endif