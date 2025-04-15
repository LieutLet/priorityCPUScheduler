#include priorityQueue.h


#include "PriorityQueue.h" 
#include "Process.h"       
#include <stdexcept>       // For std::out_of_range
#include <iostream>        // For error reporting
#include <string>          // Potentially needed 

/*
creates thread
dequeues from priority queue
does processs
loops
*/
using QueueItem = Process*;

class Scheduler {
private:
	
   	 // The Scheduler shares the ready queue in the main loop but doesn't own it.
	PriorityQueue<QueueItem>& readyQueue;

public:
	Scheduler(PriorityQueue<QueueItem>& queue) : readyQueue(queue) {
		// constructor using reference to priorityQueue
	}

	//couldnt think what else to call it
	void addReadyProcess(QueueItem process) {
	   if (!process) {
	       return;
	   }
	 // Enqueue the process using its priority.
         readyQueue.enqueue(process, process->priority);
};

QueueItem selectNextProcess() {
}

bool isReadyQueueEmpty() const {
}

QueueItem peekNextProcess() const {
}

 bool shouldPreempt(const Process* runningProcess) const{
 }








//no p-thread p-thread bad
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
