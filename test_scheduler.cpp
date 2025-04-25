#include <iostream>
#include <vector>
#include <cassert> // For using assert() for simple checks
#include <string>

// Include the components needed for the test
#include "PCB.h"
#include "PriorityQueue.h"
#include "Scheduler.h"


using namespace std;

// Helper function to make tests cleaner
void printTestHeader(const string& testName) {
    cout << "\n--- Testing: " << testName << " ---" << endl;
}

int main() {
    cout << "===== Unit Testing Scheduler Component =====" << endl;

    // 1. Create the underlying Priority Queue
    // Assuming Lower value = higher priority for the queue implementation
    PriorityQueue<QueueItem> readyQueue;

    // 2. Create the Scheduler instance, linking it to the queue
    Scheduler scheduler(readyQueue);

    // 3. Create some sample Process objects for testing
    //Using local variables - addresses will be passed
    Process p1(1, 5, 0);                                                                    // ID=1, Prio=5 (Lower)
    Process p2(2, 1, 1);                                                                    // ID=2, Prio=1 (Highest)
    Process p3(3, 3, 2);                                                                    // ID=3, Prio=3 (Medium)
    Process p4(4, 5, 3);                                                                    // ID=4, Prio=5 (Lower, same as p1)
    Process p5(5, 1, 4);                                                                    // ID=5, Prio=1 (Highest, same as p2)

    // --- Test Initial State ---
    printTestHeader("Initial Empty State");
    assert(scheduler.isReadyQueueEmpty() == true);
    assert(readyQueue.is_empty() == true);                                                  // Check underlying queue too
    assert(scheduler.peekNextProcess() == nullptr);
    assert(scheduler.selectNextProcess() == nullptr);
    assert(scheduler.shouldPreempt(nullptr) == false);                                      // No running, no ready
    assert(scheduler.shouldPreempt(&p1) == false);                                          // Running, but no ready
    cout << "Initial state tests PASSED." << endl;

    // --- Test Adding Processes ---
    printTestHeader("Adding Processes");
    scheduler.addReadyProcess(&p1);                                                         // Add P1 (Prio 5)
    assert(scheduler.isReadyQueueEmpty() == false);
    assert(readyQueue.size() == 1);
    assert(scheduler.peekNextProcess() == &p1);                                             // P1 should be the only one

    scheduler.addReadyProcess(&p2);                                                         // Add P2 (Prio 1 - Highest)
    assert(readyQueue.size() == 2);
    assert(scheduler.peekNextProcess() == &p2);                                             // P2 should now be highest

    scheduler.addReadyProcess(&p3);                                                         // Add P3 (Prio 3)
    assert(readyQueue.size() == 3);
    assert(scheduler.peekNextProcess() == &p2);                                             // P2 still highest

    scheduler.addReadyProcess(&p4);                                                         // Add P4 (Prio 5 - Same as P1)
    assert(readyQueue.size() == 4);
    assert(scheduler.peekNextProcess() == &p2);                                             // P2 still highest

    scheduler.addReadyProcess(&p5);                                                         // Add P5 (Prio 1 - Same as P2)
    assert(readyQueue.size() == 5);
    assert(scheduler.peekNextProcess() == &p2);                                             // P2 still highest (FIFO for Prio 1)
    cout << "Adding processes tests PASSED." << endl;

    // --- Test Selection Order (selectNextProcess) ---
    printTestHeader("Process Selection Order (Dequeue)");
    QueueItem selected;

    selected = scheduler.selectNextProcess();                                               // Should be P2 (Prio 1, first added)
    assert(selected == &p2);
    assert(readyQueue.size() == 4);
    cout << "Selected: Process " << selected->processID << " (Prio " << selected->priority << ")" << endl;

    selected = scheduler.peekNextProcess();                                                 // Next should be P5 (Prio 1, second added)
    assert(selected == &p5);

    selected = scheduler.selectNextProcess();                                               // Should be P5
    assert(selected == &p5);
    assert(readyQueue.size() == 3);
     cout << "Selected: Process " << selected->processID << " (Prio " << selected->priority << ")" << endl;

    selected = scheduler.peekNextProcess();                                                 // Next should be P3 (Prio 3)
    assert(selected == &p3);

    selected = scheduler.selectNextProcess();                                               // Should be P3
    assert(selected == &p3);
    assert(readyQueue.size() == 2);
     cout << "Selected: Process " << selected->processID << " (Prio " << selected->priority << ")" << endl;

    selected = scheduler.peekNextProcess();                                                 // Next should be P1 (Prio 5, first added)
    assert(selected == &p1);

    selected = scheduler.selectNextProcess();                                               // Should be P1
    assert(selected == &p1);
    assert(readyQueue.size() == 1);
     cout << "Selected: Process " << selected->processID << " (Prio " << selected->priority << ")" << endl;

    selected = scheduler.peekNextProcess();                                                 // Next should be P4 (Prio 5, second added)
    assert(selected == &p4);

    selected = scheduler.selectNextProcess();                                               // Should be P4
    assert(selected == &p4);
    assert(readyQueue.size() == 0);
    assert(scheduler.isReadyQueueEmpty() == true);
     cout << "Selected: Process " << selected->processID << " (Prio " << selected->priority << ")" << endl;

    selected = scheduler.selectNextProcess();                                               // Should be null
    assert(selected == nullptr);
    cout << "Selection order tests PASSED." << endl;

    // --- Test Preemption ---
    printTestHeader("Preemption Logic");
   
    assert(scheduler.isReadyQueueEmpty());                                                  // Reset queue

    Process running_low(10, 8, 0);                                                          // Running process, Prio 8 (Low)
    Process running_med(11, 4, 0);                                                          // Running process, Prio 4 (Medium)
    Process ready_high(12, 1, 0);                                                           // Ready process, Prio 1 (High)
    Process ready_med(13, 4, 0);                                                            // Ready process, Prio 4 (Medium)
    Process ready_low(14, 9, 0);                                                            // Ready process, Prio 9 (Low)

    // Case 1: Ready queue empty
    assert(scheduler.shouldPreempt(&running_low) == false);

    // Case 2: Ready queue has LOWER priority process
    scheduler.addReadyProcess(&ready_low);                                                  // Add Prio 9
    assert(scheduler.shouldPreempt(&running_low) == false);                                 // 9 not < 8
    assert(scheduler.shouldPreempt(&running_med) == false);                                 // 9 not < 4
    scheduler.selectNextProcess();                                                          // Clear queue

    // Case 3: Ready queue has SAME priority process
    scheduler.addReadyProcess(&ready_med);                                                  // Add Prio 4
    assert(scheduler.shouldPreempt(&running_med) == false);                                 // 4 not < 4
    scheduler.selectNextProcess();                                                          // Clear queue

    // Case 4: Ready queue has HIGHER priority process
    scheduler.addReadyProcess(&ready_high);                                                 // Add Prio 1
    assert(scheduler.shouldPreempt(&running_low) == true);                                  // 1 < 8
    assert(scheduler.shouldPreempt(&running_med) == true);                                  // 1 < 4
    assert(scheduler.shouldPreempt(&ready_high) == false);                                  // 1 not < 1 (can't preempt self)
    assert(scheduler.shouldPreempt(nullptr) == false);                                      // No running process
    cout << "Preemption tests PASSED." << endl;


    cout << "\n===== Scheduler Unit Tests Complete: ALL PASSED =====" << endl;

    return 0;
}