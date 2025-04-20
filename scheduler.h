// *********************************************************************************************************//
// File: Scheduler.h                                                                                        //
//                                                                                                          //
// Defines the Scheduler class, which handles the logic for process selection                               //
// based on priority in this OS simulation.                                                                 //
//                                                                                                          //
// This class interacts with a "PriorityQueue.h"to manage the                                               //
// set of READY processes. It offers methods to add processes to the queue, select                          //
// the next process to run according to priority rules (including preemption), and                          //
// query the state of the ready queue. Processes of the same priority are handled according to FIFO         //
// *********************************************************************************************************//
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "priorityQueue.h" 
#include "PCB.h"           
#include <stdexcept>                                                                                                    // Required for out_of_range 
#include <iostream>                                                                                                     // Required for cerr (used for error reporting)


using QueueItem = Process*;                                                                                             // Define the type of item stored in the Priority Queue and managed by the Scheduler.

// *********************************************************************************************************//
// Class: Scheduler                                                                                         //
//                                                                                                          //
// Implements the logic for a preemptive priority-based CPU scheduler.                                      //
// It uses a PriorityQueue (passed by reference) to manage the collection of ready processes.               //
// Provides methods for process addition, selection (dispatching), and preemption checks.                   //
// *********************************************************************************************************//
class Scheduler {
private:
    
    
    PriorityQueue<QueueItem>& readyQueue;                                                                               // A reference to the priority queue that holds processes in the READY state.

public:
    // ******************************************************************************************************//
    // Constructor: Initializes the scheduler.                                                               //
    //                                                                                                       //
    // Sets up the scheduler by associating it with a specific priority queue instance                       //
    // that will hold the ready processes for this simulation run.                                           //
    //                                                                                                       //
    // Parameters:                                                                                           //
    //   queue - A reference to the PriorityQueue<QueueItem> instance that the scheduler will use.           //
    //                                                                                                       //
    // ******************************************************************************************************//
    Scheduler(PriorityQueue<QueueItem>& queue) : readyQueue(queue) {                                                    // Constructor body is empty; initialization is done via the member initializer list.
        
    }

    // ******************************************************************************************************//
    // Method: addReadyProcess                                                                               //
    //                                                                                                       //
    // Adds a given process to the ready queue based on its priority value (lower value = higher priority).  //
    // This is called when a process first arrives, completes an I/O operation,                              //
    // or is preempted and needs to return to the ready state.                                               //
    //                                                                                                       //
    // Parameters:                                                                                           //
    //   process - A QueueItem (Process*) representing the process that has become ready.                    //
    //             Null pointers passed as input are safely ignored.                                         //
    //                                                                                                       //
    // Notes:                                                                                                //
    //   - Assumes the `priority` member of the passed `process` object is correctly set.                    //
    //   - Relies on the PriorityQueue's implementation to handle priority levels correctly                  //
    //     (lower numerical value = higher priority) and FIFO ordering within the same priority.             //
    // ******************************************************************************************************//
    void addReadyProcess(QueueItem process) {
        
        if (!process) {                                                                                                 // Perform a basic null check
            return;                                                                                                     // Do nothing if the process pointer is null.
        }
        readyQueue.enqueue(process, process->priority);                                                                 // Enqueue the process into the referenced readyQueue, using the process's priority value.
    }

    // ******************************************************************************************************//
    // Method: selectNextProcess                                                                             //
    //                                                                                                       //
    // Selects and removes the highest priority process from the ready queue.                                //
    // This simulates the dispatcher selecting the next process to run on the CPU.                           //
    // Also handles the case where the ready queue is empty.                                                 //
    //                                                                                                       //
    // Return:                                                                                               //
    //   - A QueueItem (Process*) pointing to the highest priority process if the queue is not empty.        //
    //   - `nullptr` if the ready queue is empty, signaling that the CPU should remain idle.                 //
    //                                                                                                       //
    // Notes:                                                                                                //
    //   - Uses a defensive try-catch block. The preceding `is_empty()` check should prevent                 //
    //     `out_of_range` from the queue's `dequeue` in normal operation, this is a just in case check       //
    //      to protect against potential internal logic errors or unexpected queue states.                   //
    // ******************************************************************************************************//
    QueueItem selectNextProcess() {
        if (readyQueue.is_empty()) {                                                                                    // Check if the queue is empty before attempting to dequeue.
            return nullptr;                                                                                             // Return nullptr if no process is available.
        } else {                                                                                                        // If queue is not empty, proceed to dequeue.
            try {
                return readyQueue.dequeue();                                                                            // Delegate to the priority queue's dequeue method, which handles priority logic.
            } catch (const out_of_range& e) {
                cerr << "CRITICAL SCHEDULER ERROR: Dequeue failed unexpectedly on non-empty queue! "                    // If this block is reached we have serious problems. Log it if it happens.
                          << e.what() << endl;
                return nullptr;                                                                                         // Return nullptr to signify failure / maintain idle state.
            }
        }
    }

    // ******************************************************************************************************//
    // Method: isReadyQueueEmpty                                                                             //
    //                                                                                                       //
    // Checks if the ready queue associated with this scheduler currently contains no more processes.        //
    // Provides a convenient way for the main simulation loop or other components to query the queue state.  //
    //                                                                                                       //
    // Return:                                                                                               //
    //   - `true` if the ready queue holds no processes.                                                     //
    //   - `false` if there is at least one process in the ready queue.                                      //
    // ******************************************************************************************************//
    bool isReadyQueueEmpty() const {
        return readyQueue.is_empty();
    }

    // ******************************************************************************************************//
    // Method: peekNextProcess                                                                               //
    //                                                                                                       //
    // Returns the highest priority process from the ready queue without removing it.                        //
    // Useful for preemption checks where the scheduler needs to compare the running process's priority      //
    // with a newly arriving process.                                                                        //
    // Also handles the case where the ready queue is empty.                                                 //
    //                                                                                                       //
    // Return:                                                                                               //
    //   - A QueueItem (Process*) pointing to the highest priority process if the queue is not empty.        //
    //   - `nullptr` if the ready queue is empty.                                                            //
    //                                                                                                       //
    // Notes:                                                                                                //
    //   - Includes a defensive try-catch similar to `selectNextProcess`.                                    //
    // ******************************************************************************************************//
    QueueItem peekNextProcess() const {         
         if (readyQueue.is_empty()) {                                                                                       // Check if the queue is empty before attempting to peek.
            return nullptr;                                                                                                 // Return nullptr if no process is available to peek at.
        } else {
             try {                                                                                                          // Queue is not empty, proceed to peek.
                return readyQueue.peek();                                                                                   // Delegate to the priority queue's peek method.
            } catch (const out_of_range& e) {
                cerr << "CRITICAL SCHEDULER ERROR: Peek failed unexpectedly on non-empty queue! "                           // Defensive catch, similar to selectNextProcess. If this happens we have serious problems
                          << e.what() << endl;
                return nullptr;                                                                                             // Return nullptr on unexpected failure.
            }
        }
    }

    // ******************************************************************************************************//
    // Method: shouldPreempt                                                                                 //
    //                                                                                                       //
    // Determines if the currently running process should be preempted by a higher-priority process          //
    // that is waiting in the ready queue.                                                                   //
    // This implements the core decision logic for preemptive priority scheduling.                           //
    //                                                                                                       //
    // Parameters:                                                                                           //
    //   runningProcess - A pointer (`const Process*`) to the process currently executing on the CPU.        //
    //                    This can be `nullptr` if the CPU is currently idle.                                //
    //                                                                                                       //
    // Return:                                                                                               //
    //   - `true` if there is a process in the ready queue that has a strictly higher priority               //
    //     (lower numerical priority value) than the `runningProcess`.                                       //
    //   - `false` if the CPU is idle, if the ready queue is empty, or if the highest priority               //
    //     process in the ready queue has the same or lower priority (equal or higher numerical value)       //
    //     than the `runningProcess`.                                                                        //
    //                                                                                                       //
    // ******************************************************************************************************//
    bool shouldPreempt(const Process* runningProcess) const {                                                               
        if (!runningProcess) {                                                                                              // Preemption is not possible if the CPU is already idle.
            return false;
        }

        QueueItem highestReady = peekNextProcess();                                                                         // Check the highest priority process waiting in the ready queue without removing it.
    
        if (!highestReady) {                                                                                                // If the ready queue is empty or peek failed unexpectedly, no preemption.
            return false;
        }

        if (highestReady->priority < runningProcess->priority) {                                                            // Compare priorities. Preempt only if the waiting process's priority value is lower
            return true;                                                                                                    // than the running process's priority value.
        } else {
            return false;                                                                                                   // The ready process has same or lower priority. Do not preempt.
        }
    }

    

}; 

#endif 