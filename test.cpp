#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include "semaphor.h"
#include <iostream.h>

Semaphore semCD=Semaphore(0);

class Slova : public Thread
{
public:
	Slova(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(stackSize,timeSlice){ id = ++tekID; }
private:
	static ID tekID;
	ID id;
	
	static Semaphore semAB;
	virtual void run();
};

ID Slova::tekID=0;
Semaphore Slova::semAB = Semaphore(0);

void Slova::run()
{
	for (int i =0; i < 30; ++i)
	{
		lockTake
		cout<<"id"<<id<<" i = "<<i<<endl;
		unlockTake
		
		if (id==2 && i==3)
		{ 
			semCD.wait();	
			//dispatch();
			lockTake
			cout<<"- id2 poziva wait"<<endl;
			unlockTake
		}
		
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	if (id==1)
	{ 
		semCD.signal();
		lockTake
		cout<<"- id1 poziva signal"<<endl;
		unlockTake
	}
	//exitThread();
}

volatile int cnt = 0;
void tick()
{
  cnt++;
}

Slova *a,*b,*m;
void doSomething()
{
	lock
	a = new Slova(1024,40);
	cout<<"napravio a"<<endl;
	a->start();
	
	b = new Slova(1024,20);
	cout<<"napravio b"<<endl;
	b->start();
	unlock

	for (int i = 0; i < 15; ++i) 
	{
		lock
		cout<<"usrMain "<<i<<endl;
		unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	lock
	cout << "cnt = "<<cnt<< endl;
	unlock
	cout << "Happy End" << endl;
}

int userMain()
{	
	doSomething();
	
	return 0;
}