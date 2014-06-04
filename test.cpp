#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include <iostream.h>

class Slova : public Thread
{
public:
	Slova(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(stackSize,timeSlice){ id = ++tekID; }
private:
	static ID tekID;
	ID id;
	virtual void run();
};

ID Slova::tekID=0;

void Slova::run()
{
	for (int i =0; i < 30; ++i)
	{
		lockCout
		cout<<"id = "<<id<<" i = "<<i<<endl;
		unlockCout
		
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
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

	m = new Slova(1024,20);
	PCB::running = m->myPCB;
	unlock

	for (int i = 0; i < 15; ++i) 
	{
		lock
		cout<<"main "<<i<<endl;
		unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	lock
	cout << "cnt = "<<cnt<< endl;
	unlock
	cout << "Happy End" << endl;
}

int main()
{
	inic();
	
	doSomething();
	
	restore();
 
	return 0;
}