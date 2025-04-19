//**********************************************************************************************//
// File: main.cpp                                                                               //
//                                                                                              //
// Main driver program for the Preemptive Priority-Based Process Scheduling Simulation.         //
// This program runs the simulation by:                                                         //
// 1. Loading process definitions from an input file.                                           //
// 2. Initializing simulation components (Scheduler, Ready Queue, Process Table).               //
// 3. Running the main simulation loop, processing events tick by tick:                         //
//    - Handling I/O completions.                                                               //
//    - Handling process arrivals.                                                              //
//    - Simulating CPU execution and preemption.                                                //
//    - Making scheduling decisions when the CPU is idle.                                       //
//    - Tracking time and process states.                                                       //
// 4. Collecting and reporting performance statistics when the simulation completes.            //
//**********************************************************************************************//

#include <iostream>
#include <vector>
#include <string>
#include <map>                                                                                                                  // For I/O completion tracking map
#include <algorithm>                                                                                                            // For sort
#include <memory>                                                                                                               // For unique_ptr (process management)
#include <iomanip>                                                                                                              // For setw, setprecision (output formatting)
#include <stdexcept>                                                                                                            // For runtime_error (exception handling)

// Include all custom component headers
#include "PCB.h"                                                                                                                // Defines Process structure and State enum
#include "processData.h"                                                                                                        // Defines structure for initial process info
#include "ProcessLoader.h"                                                                                                      // Declares function to load processes from file
#include "priorityQueue.h"                                                                                                      // Defines the PriorityQueue class template
#include "Scheduler.h"                                                                                                          // Defines the Scheduler class

using QueueItem = Process*;                                                                                                     // Defines the type of item stored in the Ready Queue and managed by the Scheduler.
                                                                                                                                // Alias for Process*, consistent with Scheduler.h definition.
using namespace std;

//*************************************************************************************************//
// Helper function to check for and handle CPU preemption.                                         //
// Determines if the currently running process should be preempted by a higher-priority            //
// process that has just become ready (via arrival or I/O completion).                             //
// If preemption occurs, the running process is moved back to the ready queue,                     //
// and the CPU is marked as idle by setting currentCpuProcess to nullptr.                          //
//                                                                                                 //
// Parameters:                                                                                     //
//          currentCpuProcess: A reference to the pointer holding the currently running process.   //
//                             -This will be set to nullptr if preemption occurs.                  //
//                                                                                                 //
//          scheduler: The Scheduler instance used to check preemption conditions and              //
//                     add the preempted process back to the ready queue.                          //
//                                                                                                 //
// Return: `true` if preemption occurred (and currentCpuProcess was set to nullptr),               //
//         `false` otherwise.                                                                      //
//*************************************************************************************************//
bool handlePreemption(QueueItem& currentCpuProcess, Scheduler& scheduler) {
    if (currentCpuProcess != nullptr && scheduler.shouldPreempt(currentCpuProcess)) {                                           // Check if CPU is busy AND if the scheduler indicates preemption is necessary
        cout << "  PREEMPTION: Process P" << scheduler.peekNextProcess()->processID                                             // Log the preemption event
                  << " (Prio " << scheduler.peekNextProcess()->priority
                  << ") preempts P" << currentCpuProcess->processID
                  << " (Prio " << currentCpuProcess->priority << ")" << endl;

        
        currentCpuProcess->currentState = State::READY;                                                                         // Update the state of the preempted process
                                                                                                                                // Its timeRemainingInBurst should be correctly set from previous execution ticks.

        scheduler.addReadyProcess(currentCpuProcess);                                                                           // Add the preempted process back to the ready queue
        currentCpuProcess = nullptr;                                                                                            // Make the CPU available for the new higher-priority process
        return true;                                                                                                            // Signal that preemption happened
    }
    return false;                                                                                                               // No preemption needed or possible (CPU idle, no higher priority process ready)
}
 
//*************************************************************************************************//
// Main entry point for the process scheduling simulation.                                         //
//*************************************************************************************************//
int main() {
    cout << "===== Process Scheduling Simulation (Priority Based) =====" << endl;

    //*************************************************************************************************//
    // --- 1. Load Process Definitions ---                                                             //
    //*************************************************************************************************//

    vector<ProcessData> loadedProcessDefinitions;
    string filename = "processes.txt";                                                                                              // Input file name
    try {                                                                                                                           // Attempt to load process data using the ProcessLoader component
        loadedProcessDefinitions = loadProcessesFromFile(filename);
        if (loadedProcessDefinitions.empty()) {                                                                                     // Basic validation: ensure some processes were actually loaded
             cerr << "Error: No valid process data loaded from " << filename << ". Exiting." << endl;
             return 1;                                                                                                              // Exit with an error code if empty
        }
        cout << "Loaded " << loadedProcessDefinitions.size() << " process definitions." << endl;
    } catch (const runtime_error& e) {                                                                                              // Catch exceptions thrown by the loader (file not found, parsing errors)
        cerr << "Error loading processes: " << e.what() << endl;
        return 1; // Exit with an error code
    }

    //*************************************************************************************************//
    // --- 2. Initialize Simulation Components & Variables ---                                         //
    //*************************************************************************************************//

    PriorityQueue<QueueItem> readyQueue;                                                                                            // The ready queue managed by the scheduler
    Scheduler scheduler(readyQueue);                                                                                                // The scheduler logic instance

    int currentTime = 0;                                                                                                            // Global simulation clock/timer
    int processesTerminated = 0;                                                                                                    // Counter for completed processes
    int totalProcesses = loadedProcessDefinitions.size();                                                                           // Total expected processes
    int totalCpuBusyTime = 0;                                                                                                       // Accumulator for CPU utilization statistic

    
    QueueItem currentCpuProcess = nullptr;                                                                                          // Represents the CPU state: holds the process currently running, or nullptr if idle

    map<int, vector<QueueItem>> ioCompletionEvents;                                                                                 // Tracks pending I/O operations. Key: time of completion, Value: list of processes completing at that time.

    vector<unique_ptr<Process>> processTable;                                                                                       // Master list holding all Process objects. Using unique_ptr for automatic memory management.

    
    sort(loadedProcessDefinitions.begin(), loadedProcessDefinitions.end(),                                                          // Sort initial process definitions by arrival time. 
              [](const ProcessData& a, const ProcessData& b) {
                  return a.arrivalTime < b.arrivalTime;
              });
    size_t nextArrivalIndex = 0;                                                                                                    // Index into the sorted loaded definitions

    cout << "\n--- Simulation Starting ---" << endl;
    
    //*************************************************************************************************//
    // --- 3. Main Simulation Loop ---                                                                 //
    //     -Keep looping as long as not all processes have terminated                                  //
    //     -Also checks if CPU is idle to prevent infinite loops                                       //
    //*************************************************************************************************//

    while (processesTerminated < totalProcesses) {
        // --- Safety Check: Detect potential infinite loops ---
        
        
        if (nextArrivalIndex == loadedProcessDefinitions.size() &&                                                                  // If no more processes will arrive, the ready queue is empty, the CPU is idle,
            readyQueue.is_empty() &&                                                                                                // and no I/O events are pending, the simulation cannot progress further.
            currentCpuProcess == nullptr &&
            ioCompletionEvents.empty()) {
                if (processesTerminated < totalProcesses) {                                                                         // If this state is reached but not all processes terminated, it indicates a potential logic error or impossible scenario.
                     cout << "\nWarning: System idle but not all processes terminated ("
                          << processesTerminated << "/" << totalProcesses
                          << "). Check process definitions or simulation logic for issues." << endl;
                }
                break;                                                                                                              // Otherwise, simulation completed on the previous tick.
        }

       
        cout << "\nTime [" << setw(3) << currentTime << "]" << endl;                                                                // Print header for the current time tick

                                                                                                                                    // Flags to manage changes within the current tick
        bool cpuBecameIdleThisTick = false;                                                                                         // Did the CPU finish a task this tick?
        bool preemptionOccurred = false;                                                                                            // Did a preemption event happen this tick?

        //************************************************************************************//
        // ------------------------- Event: I/O Completions ------------------------          //
        //      Check if any processes finish their I/O at the beginning of this time step.   //
        //************************************************************************************//
        if (ioCompletionEvents.count(currentTime)) {
            vector<QueueItem>& completedIO = ioCompletionEvents[currentTime];                                                       // Get list of processes done with I/O

                                                                                                                                    // Enforces FIFO within the same priority level for processes finishing I/O together.
            sort(completedIO.begin(), completedIO.end(), [](QueueItem a, QueueItem b) {                                             // Sort processes completing their I/O simultaneously to maintain FIFO protocol.
                if (a->priority != b->priority) {                                                                                   // Sort first by Priority (lower value first)
                    return a->priority < b->priority;                                                       
                } else if (a->arrivalTime != b->arrivalTime) {                                                                      // Sort second by arrival time (Earlier arrival first)
                    return a->arrivalTime < b->arrivalTime; 
                } else {    
                    return a->processID < b->processID;                                                                             // Final tie-break by ID (ascending)
                }
            });

            
            for (QueueItem p : completedIO) {                                                                                       // Process each completed I/O process
                p->currentState = State::READY;                                                                                     // Set state back to ready
                cout << "  I/O Done: P" << p->processID << " -> READY (Arrival: " 
                << p->arrivalTime << ")" << endl;

                scheduler.addReadyProcess(p);                                                                                       // Add back to the ready queue
                
                preemptionOccurred = handlePreemption(currentCpuProcess, scheduler) || preemptionOccurred;                          // Immediately check if this newly ready process should preempt the running one
                if (currentCpuProcess == nullptr && preemptionOccurred) {
                    cpuBecameIdleThisTick = true;                                                                                   // If Preemption occurs CPU is made idle
                }
            }
            ioCompletionEvents.erase(currentTime);                                                                                  // Remove processed I/O completion time entry
        }

        //***********************************************************************//
        // ------------------ Event: Process Arrivals --------------------       //
        //      Check for new processes arriving at the current time step.       //
        //***********************************************************************//
        while (nextArrivalIndex < loadedProcessDefinitions.size() &&
               loadedProcessDefinitions[nextArrivalIndex].arrivalTime == currentTime) {

            const ProcessData& data = loadedProcessDefinitions[nextArrivalIndex];                                                   // Get data for arriving process

            
            auto newProcess = make_unique<Process>(data.processID, data.priority, data.arrivalTime);                                // Create the runtime Process object using a unique_ptr for ownership of the process

            
            newProcess->cpuBursts = data.cpuBursts;                                                                                 // Copy CPU burst sequences from loaded data to the PCB object
            newProcess->ioBursts = data.ioBursts;                                                                                   // Copy I/O burst sequences from loaded data to the PCB object
           
            newProcess->currentState = State::READY;                                                                                // Initial state is NEW in constructor, immediately move to READY

            cout << "  Arrival:  P" << newProcess->processID                                                                        // Log arrival of process along with its priority and state
                      << " (Prio " << newProcess->priority << ") -> READY" << endl;

            QueueItem processPtr = newProcess.get();                                                                                // Get raw pointer for queue/scheduler
            processTable.push_back(move(newProcess));                                                                               // Add the owned process to the master table

            scheduler.addReadyProcess(processPtr);                                                                                  // Add the newly arrived process to the ready queue

            
            preemptionOccurred = handlePreemption(currentCpuProcess, scheduler) || preemptionOccurred;                              // Immediately check if this newly arrived process should preempt the running one
            if (currentCpuProcess == nullptr && preemptionOccurred) {
                cpuBecameIdleThisTick = true;                                                                                       // If preemption is made CPU is made idle
            }

            nextArrivalIndex++;                                                                                                     // Move to the next potential arrival
        }

        //***********************************************************************//
        // ----------------- Update State of the Running Process --------------  //
        //***********************************************************************//
        if (currentCpuProcess != nullptr) {
            currentCpuProcess->cpuTimeExecuted++;                                                                                   // Increment total CPU time for this process
            totalCpuBusyTime++;                                                                                                     // Increment global CPU busy time counter
            currentCpuProcess->timeRemainingInBurst--;                                                                              // Decrement remaining time for the current CPU burst

            cout << "  CPU Running: P" << currentCpuProcess->processID
                      << " (Prio " << currentCpuProcess->priority << "), Burst Remaining: "
                      << currentCpuProcess->timeRemainingInBurst << endl;

            
            if (currentCpuProcess->timeRemainingInBurst == 0) {                                                                     // Check for CPU Burst Completion 
                cout << "  CPU Burst Finished: P" << currentCpuProcess->processID << endl;
                currentCpuProcess->currentBurstIndex++;                                                                             // Advance to the next burst index

                
                if (currentCpuProcess->currentBurstIndex >= currentCpuProcess->cpuBursts.size()) {                                  // Check if the process terminates
                    
                    currentCpuProcess->currentState = State::TERMINATED;                                                            // Update state of current process to TERMINATED

                    
                    currentCpuProcess->completionTime = currentTime + 1;                                                            // Record final statistics at the END of the current time tick
                    currentCpuProcess->turnaroundTime = currentCpuProcess->completionTime - currentCpuProcess->arrivalTime;

                    
                    int totalIoDuration = 0;                                                                                        // Calculate total I/O time for this process (sum of completed IO bursts)
                    for(size_t i = 0; i < currentCpuProcess->ioBursts.size(); ++i) {
                        if (i < currentCpuProcess->currentBurstIndex - 1) {                                                         // Only sum I/O bursts corresponding to CPU bursts that actually finished
                           totalIoDuration += currentCpuProcess->ioBursts[i];
                        }
                    }
                    currentCpuProcess->ioTime = totalIoDuration;

                    currentCpuProcess->waitingTime = currentCpuProcess->turnaroundTime - currentCpuProcess->cpuTimeExecuted - currentCpuProcess->ioTime;    // Calculate waiting time based on other stats
               
                    cout << "  TERMINATED: P" << currentCpuProcess->processID
                              << " at T=" << currentCpuProcess->completionTime << endl;

                    currentCpuProcess = nullptr;                                                                                    // Free the CPU
                    cpuBecameIdleThisTick = true;                                                                                   // Mark that CPU became idle this tick
                    processesTerminated++;                                                                                          // Increment terminated processes counter
                } else {
                    //***********************************************************************//
                    // ---------------------- Process Blocks for I/O ----------------------  //
                    //***********************************************************************//
                    currentCpuProcess->currentState = State::WAITING;

                    
                    int ioDuration = currentCpuProcess->ioBursts[currentCpuProcess->currentBurstIndex - 1];                         // Get the duration of the upcoming I/O burst (index should be equal to the CPU burst just finished)
                    currentCpuProcess->timeRemainingInBurst = ioDuration;                                                           // Store I/O duration (for statistics if we want)
                    int completionTime = currentTime + 1 + ioDuration;                                                              // Calculate I/O completion time (starts *after* the current tick)
                    
                    ioCompletionEvents[completionTime].push_back(currentCpuProcess);                                                // Schedule the I/O completion event in the map

                    cout << "  Blocked:    P" << currentCpuProcess->processID << " for I/O (Duration: " <<
                     ioDuration << ") until T=" << completionTime << endl;

                    currentCpuProcess = nullptr;                                                                                    // Free the CPU
                    cpuBecameIdleThisTick = true;                                                                                   // Mark that CPU became idle this tick
                }
            }
        } else {                                                                                                                    //else: print CPU Idle if the cpu is idle for consecutive ticks
             if (!cpuBecameIdleThisTick) {
                 cout << "  CPU Idle" << endl;
             }
        }
        //***********************************************************************//
        // ------------------------ Scheduling Decision ----------------------   //
        //***********************************************************************//
        if (currentCpuProcess == nullptr) {                                                                                         //If the CPU is currently idle, attempt to select next process from the ready queue
            if (!scheduler.isReadyQueueEmpty()) {
                QueueItem nextProcess = scheduler.selectNextProcess();                                                              // Select and remove the highest priority available process 
                if (nextProcess != nullptr) {                                                                                       // Check if ready queue is empty
                    currentCpuProcess = nextProcess;                                                                                // Dispatch process to CPU 
                    currentCpuProcess->currentState = State::RUNNING;

                    currentCpuProcess->timeRemainingInBurst = currentCpuProcess->cpuBursts[currentCpuProcess->currentBurstIndex];   // Set the time required for the *next* CPU burst of this process

                    if (currentCpuProcess->startTime == -1) {                                                                       // Record start time and response time if this is the process's *first* run
                        currentCpuProcess->startTime = currentTime; 
                        currentCpuProcess->responseTime = currentCpuProcess->startTime - currentCpuProcess->arrivalTime;
                    }

                    cout << "  Dispatch:   P" << currentCpuProcess->processID
                              << " (Prio " << currentCpuProcess->priority << ") to CPU (Burst: "
                              << currentCpuProcess->timeRemainingInBurst << ")" << endl;
                }
            }                                                                                                                       // else: CPU remains idle as the ready queue is empty.
        }
        //***********************************************************************//
        // ----------- Testing & Debugging: Print Ready Queue State------------  //
        //  -Uncomment during testing to trace queue contents                    //
        //***********************************************************************//
        // cout << "  Ready Queue Size: " << readyQueue.size() << endl;
        // cout << "  Ready Queue Contents: " << readyQueue.toString() << endl; // Requires suitable toString in PriorityQueue

        currentTime++;                                                                                                              // Advance simulation clock 


    }                                                                                                                               // End main simulation loop
    //***********************************************************************//
    // --------------------- Simulation Completion ---------------------     //
    //***********************************************************************//
    int finalTime = currentTime - 1;                                                                                                 // Record the final simulation time
    cout << "\n--- Simulation Finished at Time: " << finalTime << " ---" << endl;

    //************************************//
    // ------ 5. Report Results ------    //
    //************************************//
    cout << "\n===== Final Statistics =====" << endl;
    cout << "Total processes simulated: " << totalProcesses << endl;
    cout << "Total processes terminated: " << processesTerminated << endl;                                                          // Final count of terminated processes (should match totalProcesses)

    
    if (processesTerminated > 0) {                                                                                                  // Calculate and print average statistics if  all processes actually finished
        double totalTurnaroundTime = 0;
        double totalWaitingTime = 0;
        double totalResponseTime = 0;
       
        //***********************************************************//
        // ------ Print Individual Process Statistics Table ------   //
        //***********************************************************//
        cout << "\n--- Individual Process Statistics ---" << endl;
        cout << setw(6) << "PID |" << setw(9) << "Arrival |" << setw(9) << "Start |" << setw(12) << "Completion |"
             << setw(13) << "Turnaround |" << setw(11) << "Waiting |" << setw(11) << "Response |" << setw(11) << "CPU Time |"
              << setw(11) << "IO Time" << endl;
        cout << setfill('-') << setw(95) << "" << setfill(' ') << endl; // Separator line

       
        for (const auto& proc_ptr : processTable) {                                                                                 // Iterate through the master process table to gather final stats
             cout << setw(4) << proc_ptr->processID << " |"                                                                         // Format output for each process's statistics
                  << setw(7) << proc_ptr->arrivalTime << " |"
                  << setw(7) << (proc_ptr->startTime == -1 ? "N/A" : to_string(proc_ptr->startTime)) << " |"                        // Display "N/A" if process never started or finished
                  << setw(10) << (proc_ptr->completionTime == -1 ? "N/A" : to_string(proc_ptr->completionTime)) << " |"
                  << setw(11) << (proc_ptr->completionTime == -1 ? "N/A" : to_string(proc_ptr->turnaroundTime)) << " |"
                  << setw(9) << (proc_ptr->completionTime == -1 ? "N/A" : to_string(proc_ptr->waitingTime)) << " |"
                  << setw(9) << (proc_ptr->responseTime == -1 ? "N/A" : to_string(proc_ptr->responseTime)) << " |"
                  << setw(9) << proc_ptr->cpuTimeExecuted << " |"                                                                   // Show executed time
                  << setw(9) << proc_ptr->ioTime << endl;                                                                           // Show calculated I/O time

            
            if (proc_ptr->currentState == State::TERMINATED && proc_ptr->completionTime != -1) {                                    // Accumulate stats for average calculations, only count processes that actually terminated
                totalTurnaroundTime += proc_ptr->turnaroundTime;
                totalWaitingTime += proc_ptr->waitingTime;
        
                if (proc_ptr->responseTime != -1) {                                                                                 // Only include response time in average if the process actually ran
                   totalResponseTime += proc_ptr->responseTime;
                } else if (proc_ptr->cpuTimeExecuted > 0) {
                    cerr << "Warning: Terminated process P" << proc_ptr->processID <<
                     " used CPU but has no response time." << endl;                                                                 //Error handling if a terminated process somehow has no response time
                } 

            } else if (proc_ptr->currentState != State::TERMINATED) {
                 cerr << "Warning: Process P" << proc_ptr->processID << " did not terminate. Final state: " <<
                  static_cast<int>(proc_ptr->currentState) << endl;                                                                 // Error handling if any process didn't terminate by the end of the simulation
            }
        }

        //***********************************************************//
        //    ------------- Print Average Statistics -------------   //
        //***********************************************************//
        cout << "\n--- Averages ---" << endl;
        cout << fixed << setprecision(2);                                                                                           // Format averages to two decimal places
        cout << "Average Turnaround Time: " << (totalTurnaroundTime / processesTerminated) << endl;                                 // Calculate and print averages
        cout << "Average Waiting Time:    " << (totalWaitingTime / processesTerminated) << endl;
        cout << "Average Response Time:   " << (totalResponseTime / processesTerminated) << endl;                               

        
        if (finalTime > 0) {                                                                                                        // Calculate and print CPU utilization
             double cpuUtilization = static_cast<double>(totalCpuBusyTime) / finalTime * 100.0;
             cout << "CPU Utilization:         " << cpuUtilization << "%" << endl;
        } else {
             cout << "CPU Utilization:         N/A (Simulation time is 0)" << endl;
        }
         cout << "Total CPU Busy Time:     " << totalCpuBusyTime << " ticks" << endl;
         cout << "Total Simulation Time:   " << finalTime << " ticks" << endl;

    } else {
        cout << "No processes terminated, cannot calculate average statistics." << endl;                                            // Error handling if no processes finished (if input file was empty, or didnt contain valid processes)
    }

    return 0; 
}