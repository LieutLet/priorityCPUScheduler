//****************************************************************************************************//
//  processData.h  -  Immutable process description loaded before the simulation starts               //
//                                                                                                    //
//  Each input line in *processes.txt* is parsed into one ProcessData structure.  This header keeps   //
//  that structure minimal and strictly read-only: the simulator must *never* modify a ProcessData    //
//  instance.  When the clock hits `arrivalTime`, the dispatcher constructs a live Process (see       //
//  Process.h) by copying these fields and then adds run-time state to the new PCB.                   //
//                                                                                                    //
//  Separating ProcessData from the PCB provides two big wins:                                        //
//      Loader logic is isolated and simple to unit-test.                                             //
//      The scheduler cannot accidentally tamper with constants such as the original burst lengths.   //
//****************************************************************************************************//

#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include <vector>                                                                              

// Structure to hold the initial definition of a process read from a file.
struct ProcessData {
    int processID;
    int arrivalTime;
    int priority;                                                                                               // Lower value = higher priority
    std::vector<int> cpuBursts;                                                                                 // Sequence of CPU burst durations
    std::vector<int> ioBursts;                                                                                  // Sequence of IO burst durations

    
    ProcessData(int id = -1, int arrival = -1, int prio = -1) :                                                 // Constructor with default arguments covers the default case
        processID(id), arrivalTime(arrival), priority(prio) {}
};

#endif // PROCESS_DATA_H