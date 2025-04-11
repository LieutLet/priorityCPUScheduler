#include priorityQueue.h

//was told to use these for pthreads
#include <stdio.h>
#include <pthread.h>

/*
creates thread
dequeues from priority queue
does processs
loops
*/

class Schedular {
public:
	//just made everthing public to avoid any errors for now
	priorityQueue Q;

	//couldnt think what else to call it (really starting to think this class should just be our main function
	void run();

};

void Schedular::run() {
	//the while loop should keep going so long as there is something in the priority queue
	while (!Q.is_empty) {
		//create thread for process
		//do process
		//pthread_join
	}
}
