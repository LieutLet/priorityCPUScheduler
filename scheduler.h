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

class Scheduler {
private:
	//the priorityQueue in question
	priorityQueue Q;

public:
	Scheduler(priorityQueue& queue) : Q(queue) {
		// constructor using reference to priorityQueue
	}

	//couldnt think what else to call it
	void schedule:
};

void Scheduler::schedule() {
	int i = 0;
	//creates thread for each node in the priorityqueue
	pthread_t tid[Q.size()];

	//the while loop should keep going so long as there is something in the priority queue
	while (!Q.is_empty) {
		// create threads for dequeue
		pthread_create(&tid[i], NULL, Q.dequeue(), NULL);
		//increment i for joining (see below)
		i++;
	}

	while (i >= 0) {
		//joins all threads
		pthread_join(tid[i], NULL);
	}
}
