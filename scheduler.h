#include priorityQueue.h


#include "PriorityQueue.h" 
#include "Process.h"       
#include <stdexcept>       // For std::out_of_range
#include <iostream>        // For error reporting
#include <string>          // Potentially needed 


using QueueItem = Process*;

class Scheduler {
private:
	
   	 // The Scheduler shares the ready queue in the main loop but doesn't own it.
	PriorityQueue<QueueItem>& readyQueue;

public:
      /**
        * Constructor: Initializes the scheduler with a reference to the
        *              priority queue it will manage.
        *
        * Parameters:
        *      queue - A reference to the PriorityQueue instance holding ready processes.
        */
	Scheduler(PriorityQueue<QueueItem>& queue) : readyQueue(queue) {
		// this is done nothing to add
	}

      /**
        * Description: Adds a process that has become ready (e.g., arrived or
        *              finished I/O) to the ready queue based on its priority.
        *
        * Parameters:
        *      process - A pointer to the process becoming ready.
        *                Null pointers are ignored.
        */
	void addReadyProcess(QueueItem process) {
	   //NULL Check
	   if (!process) {
	       return;
	   }
	 // Enqueue the process using its priority.
         readyQueue.enqueue(process, process->priority);
	}

      /**
        * Description: Selects and removes the highest priority process from the
        *              ready queue. Handles the case where the queue is empty.
        *
        * Return:
        *      A pointer to the highest priority process if the ready queue is not empty.
        *      nullptr if the ready queue is empty.
        */
	QueueItem selectNextProcess() {
	}

      /**
        * Description: Checks if the ready queue is currently empty.
        *              
        *
        * Return:
        *      true - If there are no processes in the ready queue.
        *      false - Otherwise.
        */
	bool isReadyQueueEmpty() const {
		return readyQueue.is_empty(); // this is done, nothing more to add
	}

      /**
        * Description: Returns a pointer to the highest priority process
        *              without removing it from the queue. Useful for preemption checks.
        *              Handles the case where the queue is empty.
        *
        * Return:
        *      A pointer to the highest priority process if the ready queue is not empty.    
        *      nullptr if the ready queue is empty.
        */
	QueueItem peekNextProcess() const {
	}

      /**
        * Description: Determines if the currently running process should be
        *              preempted by a higher-priority process in the ready queue.
        *              Assumes lower priority value means higher priority.
        *
        * Parameters:
        *      runningProcess - A pointer to the process currently executing on the CPU. Can be null if CPU is idle.
        *                       
        * Return:
        *      true - If a process in the ready queue has strictly higher priority
        *             (lower priority value) than the running process.
        *      false - Otherwise (including if CPU is idle, ready queue is empty,
        *              or highest ready process has same or lower priority).
        */
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
