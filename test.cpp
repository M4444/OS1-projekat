#include "thread.h"
#include "PCB.h"
#include "KERNEL.h"
#include "semaphor.h"
#include <iostream.h>
#include "IVTEntry.h"

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

PREPAREENTRY(9, 0);

char keymap[128] = { 
  0 , 27,'1','2','3','4','5','6','7','8','9','0','-','=', 8 , 9, 
 'q','w','e','r','t','y','u','i','o','p','[',']', 13, 0 ,'a','s',
 'd','f','g','h','j','k','l',';',0,0,'`','\\','z','x','c','v','b',
 'n','m',',','.','/', 0 ,'*', 0 ,' '
};

class KeyboardEvent :public Thread{
public:
	KeyboardEvent():Thread(4096,0){bi = 0;}
	char *buff() { return buffer; }
protected:
	virtual void run();
	char znak;
private:
	char buffer[4];
	int bi;
};

void KeyboardEvent::run()
{
	lockTake
	Event event9(9);
	cout<<"KeyebordListener started!"<<endl;
	unlockTake
	char scancode, status, znak; 
	
	do
	{
		event9.wait();
		status = inportb(0x64); // ocitava statusni reg. sa 64h
		if (status & 0x01)		// Can I read?
		{  
			scancode = inportb(0x60);
			if (scancode&0x80) 
			{
				buffer[bi] = keymap[scancode&0x7F];	
				bi++;
			}
		};
   //////////////////////
		asm{
			cli
			in      al, 61h         //; Send acknowledgment without
			or      al, 10000000b   //;   modifying the other bits.
			out     61h, al         //;
			and     al, 01111111b   //;
			out     61h, al         //;
			mov     al, 20h         //; Send End-of-Interrupt signal
			out     20h, al         //;
			sti
		}
		
	} while(status & 0x01); //dok se ocitava takav status da je pritisnut neki taster
}

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
	
	KeyboardEvent* kev;
	kev = new KeyboardEvent();
	kev->start();
	unlock
	
	for (int i = 0; i < 15; ++i) 
	{
		lockTake
		cout<<"usrMain "<<i<<endl;
		unlockTake

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	lockTake
	cout << "cnt = "<<cnt<< endl;
	cout << "Happy End" << endl;
	
	cout<<"Kev: ";
	cout<<kev->buff()<<endl;
	unlockTake
}

int userMain(int argc, char* argv[])
{	
	lockTake
	cout<<"Primljeni argumenti: ";
	for (int ii=1; ii<argc; ii++)
	{
		cout<<argv[ii]<<" ";
	}
	cout<<endl;
	unlockTake
	doSomething();
	
	return 0;
}