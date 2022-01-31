// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

//#define HW1_SEMAPHORES
#define HW1_LOCKS

#include "copyright.h"
#include "system.h"
#include "synch.h"


// testnum is set in main.cc
int testnum = 1;
int numThreadsActive;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
int SharedVariable;

#ifdef HW1_SEMAPHORES
Semaphore *semaphore = new Semaphore("fred", 1);
Semaphore *barrier = new Semaphore("tim", 0);
#endif

#ifdef HW1_LOCKS
Lock *lock = new Lock("brock the lock");
#endif

void SimpleThread(int which) {
	int num, val;

	for (num=0; num < 5; num++) {
		#ifdef HW1_SEMAPHORES
	  semaphore->P();
    #endif

		#ifdef HW1_LOCKS
		lock->Acquire();
		#endif

		val = SharedVariable;
		printf("*** thread %d sees value %d\n", which, val);
		currentThread->Yield();
		SharedVariable = val+1;
		if(num == 4) numThreadsActive--;
		#ifdef HW1_SEMAPHORES
		semaphore->V();
		#endif

		#ifdef HW1_LOCKS
		lock->Release();
		#endif

		currentThread->Yield();
	}
	#ifdef HW1_SEMAPHORES
	if (numThreadsActive > 0) {
		barrier->P();
	}
	barrier->V();
	#endif

	val = SharedVariable;
	printf("Thread %d sees final value %d\n", which, val);
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest(int n) {
	DEBUG('t', "Entering SimpleTest");
	Thread *t;
	numThreadsActive = n;
	printf("NumThreadsActive = %d\n", numThreadsActive);

	for(int i=1; i<n; i++)
	{
		t = new Thread("Forked thread");
		t->Fork(SimpleThread,i);
	}
	SimpleThread(0);
}

//{
  //  switch (testnum) {
   // case 1:
	//ThreadTest1();
//	break;
  //  default:
//	printf("No test specified.\n");
//	break;
  //  }
//}

