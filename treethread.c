// Also, submit a file treethread.c that is like hello.c (Figure 12.13), 
// except that it creates and reaps 2**N - 2 joinable threads, 
// where N is a positive command-line argument. 
// These threads should be created as a binary tree rooted in the main thread. 
// The 2**(N - 1) leaf threads should simply print "Hello, world\n" and return; 
// the other threads should create and reap their two children before printing "Hello, world\n" then returning. 
// The main thread counts as a thread and so should print too. If N=1, the main thread is a leaf.


// #include <csapp.h> // for threads!(?)
#include <stdlib.h>  // for strtol
#include <stdio.h> // for printf
#include <pthread.h> // threads!

void* thread_routine(void *N); //

struct thread_info{
	long N;
	pthread_t tid_l;
	pthread_t tid_r;
};


int 
main(int argc, char* argv[])
{
	if(argc != 2) //too many arguments supplied
	{
		printf("Invalid invocation of treethread.c: Must only provide one argument (a positive integer).");
		return 1;
	}
	char* p; //set by strtol to the next position in argv[1] after the supposed number

	long N = strtol(argv[1],&p, 10); //base 10
	if (N <= 0) //invalid or negative conversion of second argument
	{
		printf("Invalid command-line argument: argument must be a positive integer.");
		return 2;
	}


	// if (N == 1) // no need to split
	// {
	// 	printf("Hello, world\n");
	// 	return 0;
	// }


	// if we're here, we're good to go
	//thread_info* ti;

	thread_routine(&N);

}


void*
thread_routine(void *N)
{
	struct thread_info ti;
	ti.N = *((long *)N); //copy over shared argument so not stepping on each other's toes

	void *p; // dummy arg to pacify pthread_join (will not actually use anything)

	// create threads, who need to know when to stop splitting

	if(ti.N > 1) //should make new threads
	{
		ti.N-=1; //decrement (so the new threads have proper value)
		//form binary tree of threads
		pthread_create(&ti.tid_l, NULL, thread_routine, &ti.N);
		pthread_create(&ti.tid_r, NULL, thread_routine, &ti.N);
		//reap threads
		int l_wait = pthread_join(ti.tid_l, &p);
		int r_wait = pthread_join(ti.tid_r, &p);
		int bad = (l_wait || r_wait); //pthread_wait returns 0 if OK
		if(bad)
		{
			printf("Problems waiting for thread! Thread who was waiting: %d", pthread_self());
			pthread_cancel(pthread_self());
		}
	}

	// done waiting for threads. Can print now
	printf("Hello, world\n");


}