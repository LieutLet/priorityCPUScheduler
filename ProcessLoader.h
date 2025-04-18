//****************************************************************************************************//
//  ProcessLoader.h  -  Utility for reading process definitions from a plain-text file                //
//                                                                                                    //
//  The simulation expects an external text file (default: processes.txt) where each non-comment      //
//  line has the format:                                                                              //
//                                                                                                    //
//      PID  ARRIVAL  PRIORITY  CPU1  IO1  CPU2  IO2  ...  CPUn                                       //
//                                                                                                    //
//  Burst values must be strictly positive integers. The loader converts that human-readable          //    
//  representation into a vector of immutable ProcessData structures (see processData.h).             //   
//                                                                                                    //
//****************************************************************************************************//

#ifndef PROCESS_LOADER_H
#define PROCESS_LOADER_H

#include <vector>
#include <string>
#include "processData.h"                                                                                    // Include the structure definition
using namespace std;

//**********************************************************************************************//
// Reads and validates the given file, then returns one ProcessData object per valid line       //
//                                                                                              //
// Throws runtime_error for any of these conditions:                                            //
//      - File cant be opened                                                                   //
//      - A line contains non-integer tokens when integers are expected                         //
//      - A burst length is zero or negative                                                    //
//      - The number of I/O bursts is greater than number of CPU bursts                         //
//**********************************************************************************************//
vector<ProcessData> loadProcessesFromFile(const string& filename);

#endif // PROCESS_LOADER_H