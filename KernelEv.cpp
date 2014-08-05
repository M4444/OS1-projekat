#include "KernelEv.h"

void KernelEv::wait()
{
	ulaz->wait();
}

void KernelEv::signal()
{
	ulaz->signal();
}