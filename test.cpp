#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include "semaphor.h"
#include <iostream.h>

class Slova : public Thread
{
public:
	Slova(char c,StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(stackSize,timeSlice)
	{ ime = c; }
	Slova(Slova *nit, char c, StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice):Thread(stackSize,timeSlice)
	{ 
		ime = c;
		nitZaWait = nit;
	}
private:
	char ime;
	
	static Semaphore semAB;
	virtual void run();
	
	Slova *nitZaWait;
};

Semaphore Slova::semAB = Semaphore(0);

void Slova::run()
{
	for (int i =0; i < 30; ++i)
	{
		lockTake
		cout<<ime<<" i = "<<i<<endl;
		unlockTake
		
		if (ime=='b' && i==2)
		{	
			lockTake
			//cout<<"- "<<ime<<" poziva wait"<<endl;
			cout<<"- "<<ime<<" poziva waitToComplete na "<<nitZaWait->ime<<endl;
			unlockTake
			//semAB.wait();
			nitZaWait->waitToComplete();
			//nitZaWait->Slova::~Slova();
		}
		
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	if (ime=='c')
	{ 
		semAB.signal();
		lockTake
		cout<<"- "<<ime<<" poziva signal"<<endl;
		unlockTake
	}
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
	a = new Slova('a',1024,40);
	cout<<"napravio a"<<endl;
	a->start();
	
	b = new Slova(a,'b',1024,20);
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