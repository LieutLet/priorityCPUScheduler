// *********************************************************************************************************//
// File: Process.h                                                                                          //
//                                                                                                          //
// Defines the structure representing a Process Control Block (PCB)                                         //
// for the operating system simulation.                                                                     //
//                                                                                                          //
// This file contains the definition for the `Process` struct, which holds                                  //
// all static characteristics (like ID, priority, burst times) and dynamic                                  //
// information (like current state, remaining burst time, statistics) needed                                //
// to manage and simulate a process. It also defines the `State` enumeration                                //
// listing the possible states a process can be in during its lifecycle.                                    //
// *********************************************************************************************************//
 #ifndef PROCESS_H
 #define PROCESS_H
 
 #include <vector>    // Required for vector (burst sequences)
 #include <string>    // Often included alongside vector/iostream, potentially useful
 #include <limits>    // Required for initializing statistics like startTime
 using namespace std;
 
 // *********************************************************************************************************//
 // Defines the possible states a process can have during its lifecycle                                      //
 // within the simulation. These states are used to track the progress and                                   //
 // current activity of each process.                                                                        //
 // *********************************************************************************************************//
 enum class State {
     NEW,                                                                                                          //Process has been created/defined but not entered into the ready queue by the scheduler
     READY,                                                                                                        //Waiting in the ready queue for its turn to use the CPU
     RUNNING,                                                                                                      //Currently executing instructions on the CPU
     WAITING,                                                                                                      //Unable to proceed until I/O operation completes (also called Blocked)
     TERMINATED                                                                                                    //Process has finished execution and exits the system
 };
 
 // *********************************************************************************************************//
 // This struct represents a Process Control Block (PCB) within the OS simulation.                           //
 //                                                                                                          //
 // This structure contains all necessary information to manage and track a single                           //
 // process throughout its lifecycle. It includes identifying information (PID), scheduling                  //
 // parameters (priority), resource requirements (CPU/IO bursts), the current execution state,               //
 // and performance metrics collected during the simulation.                                                 //
 //**********************************************************************************************************//
 struct Process {
 
     // --- Static Characteristics ---
     // These values are set when the process is created (loaded from the input file)
     int processID;                                                                                                // A unique integer used to identify this specific process
     int priority;                                                                                                 // Scheduling priority of the process. Convention: Lower value = Higher priority
     int arrivalTime;                                                                                              // The time tick when a process arrives in the system
     
     vector<int> cpuBursts;                                                                                        // Ordered sequence of CPU execution times required by the process, before needing to wait for I/O 
                                                                                                                   // Alternates with ioBursts vector
 
     vector<int> ioBursts;                                                                                         // Ordered sequence of I/O operation durations. Each duration is the I/O wait time after the completion
                                                                                                                   // of the the corresonding CPU burst (ioBursts[i] follows cpuBursts[i])
 
 
     // --- Dynamic State ---
     // These values change throughout the simulation as the process executes, waits, or changes state.
 
     State currentState;                                                                                           //The current state of the process (READY, RUNNING, WAITING, TERMINATED, etc.)
 
     
     int currentBurstIndex;                                                                                        //index indicating the next burst to be processed
                                                                                                                   // If RUNNING or READY, this points to the index in cpuBursts
                                                                                                                   // If WAITING, this points to the index in cpuBursts corresponding to the CPU burst after the current I/O operation
 
    
     int timeRemainingInBurst;                                                                                     //Remaining time for the process's current activity
                                                                                                                   // If RUNNING: time remaining is for the current CPU burst
                                                                                                                   // If WAITING: time remaining is for the current I/O operation.
 
     // --- Statistics ---
     // These values are calculated and updated during the simulation to measure performance.

     int startTime = -1;                                                                                           // The time tick when the process first transitions from READY to RUNNING. -1 means it has never run
                                                                                                                   // Used to calculate response time
 
     int completionTime = -1;                                                                                      // Time tick whent he process transitions to TERMINATED. -1 if unfinished
                                                                                                                   // Used to calculate turnaround time

     int turnaroundTime = 0;                                                                                       // completionTime - arrivalTime. Total time the process was in the system
     int waitingTime = 0;                                                                                          // Total time spent in the READY state waiting for the CPU. (turnaround time - CPU time - I/O time
     int responseTime = -1;                                                                                        // Time from process Arrival til it  starts running on the CPU (startTime - arrivalTime)
     int cpuTimeExecuted = 0;                                                                                      // Total time a process spent in RUNNING state. 
     int ioTime = 0;                                                                                               // Total time spent in the WAITING state (blocked for I/O)
 
 
     // ******************************************************************************************************//
     // Constructor: Creates a new Process object.                                                            //
     // Initializes the essential static characteristics provided as arguments.                               //
     // Sets the initial dynamic state, usually to NEW or READY depending on simulation setup.                //
     // Initializes statistics members to their default 'not yet started/completed' values (0 or -1).         //
     //                                                                                                       //
     // Parameters:                                                                                           //
     //   id:      The unique integer identifier for the process.                                             //
     //   prio:    The scheduling priority (lower value = higher priority).                                   //
     //   arrival: The simulation time tick when the process arrives.                                         //
     // ******************************************************************************************************//
     Process(int id, int prio, int arrival) :
         processID(id),
         priority(prio),
         arrivalTime(arrival),
         // Initialize dynamic state
         currentState(State::NEW),                                                                                 // Processes generally start as NEW conceptually
         currentBurstIndex(0),                                                                                     // Start expecting the first burst (index 0)
         timeRemainingInBurst(0),                                                                                  // No burst is active initially
         // Initialize statistics to default values        
         startTime(-1),                                                                                            // Hasn't started
         completionTime(-1),                                                                                       // Hasn't completed
         turnaroundTime(0),                                                                                        // Zero until completion
         waitingTime(0),                                                                                           // Zero until calculated
         responseTime(-1),                                                                                         // Hasn't responded
         cpuTimeExecuted(0),                                                                                       // No CPU time used yet
         ioTime(0)                                                                                                 // No I/O time spent yet
     {}
 
 }; 
 
 #endif // PROCESS_H