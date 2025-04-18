//--------------------------------------------------------------------------------------------------
//  ProcessLoader.cpp  -  Implementation of the plain-text process definition parser
//
//  Convert each non-comment line of a human-readable text file (default: processes.txt) into a
//  fully-validated `ProcessData` structure (see processData.h).  The simulator calls
//  `loadProcessesFromFile()` once at start-up and then treats the returned vector as immutable
//  job descriptions.
//
//  File format enforced here:
//  -------------------------
//      PID  ARRIVAL  PRIORITY  CPU1  IO1  CPU2  IO2 ...  CPUn
//  -  Tokens are whitespace-delimited decimal integers.
//  -  The burst list must start with a CPU value and then alternate CPU/IO.
//  -  Each burst length must be >0.
//  -  The I/O-burst count can be equal to the CPU-burst count or one fewer (a final CPU burst).
//  -  Blank lines and lines beginning with '#' are ignored.
//
//
//--------------------------------------------------------------------------------------------------

#include "ProcessLoader.h"
#include <fstream>                                                                                                                                  // For file input (ifstream)
#include <sstream>                                                                                                                                  // For parsing lines (stringstream)
#include <stdexcept>                                                                                                                                // For runtime_error
#include <iostream>                                                                                                                                 // For potential error messages (cerr)
#include <string>                                                                                                                                   // For to_string
using namespace std;

vector<ProcessData> loadProcessesFromFile(const string& filename) {
    vector<ProcessData> processList;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        throw runtime_error("Error: Could not open process definition file: " + filename);                                                          // Throw an error if file cant be opened
    }

    string line;
    int lineNumber = 0;
    while (getline(inputFile, line)) {
        lineNumber++;
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));                                                                                       // Trim leading/trailing whitespace
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        
        if (line.empty() || line[0] == '#') {                                                                                                       // Ignore empty lines or lines starting with '#' (comments)
            continue;
        }

        stringstream ss(line);
        ProcessData currentProcessData;
        int burstValue;
        bool readingCpu = true;                                                                                                                     // Start by expecting a CPU burst

       
        if (!(ss >> currentProcessData.processID >> currentProcessData.arrivalTime >> currentProcessData.priority)) {                               // Try reading the fixed initial fields: PID, arrival, priority
             inputFile.close(); // Close file before throwing
             throw runtime_error("Error parsing ProcessID, ArrivalTime, or Priority on line " + to_string(lineNumber));
        }

        
        while (ss >> burstValue) {                                                                                                                  // Read alternating CPU and I/O bursts
            if (burstValue <= 0) {                                                                                                                  // Validation: bursts should be positive
                 inputFile.close();
                 throw runtime_error("Error: Non-positive burst time (" + to_string(burstValue) + ") on line " + to_string(lineNumber));
            }

            if (readingCpu) {
                currentProcessData.cpuBursts.push_back(burstValue);
            } else {
                currentProcessData.ioBursts.push_back(burstValue);
            }
            readingCpu = !readingCpu;                                                                                                               // Alternate for the next read
        }

      
        if (ss.fail() && !ss.eof()) {                                                                                                               // Check if parsing failed mid-bursts (non-integer value found)
            inputFile.close();
            throw runtime_error("Error parsing burst sequence on line " + to_string(lineNumber) + ". Expected integer.");
        }

        
        if (currentProcessData.cpuBursts.empty()) {                                                                                                 // Validation: Must have at least one CPU burst
             inputFile.close(); 
            throw runtime_error("Error: No CPU bursts defined for process on line " + to_string(lineNumber));
        }

        
        
        if (currentProcessData.ioBursts.size() > currentProcessData.cpuBursts.size() ||                                                             // Validation: Number of IO bursts should be <= number of CPU bursts
            currentProcessData.ioBursts.size() < currentProcessData.cpuBursts.size() - 1) {                                                         // and at most one less than CPU bursts.
             inputFile.close();
            throw runtime_error("Error: Mismatch between CPU and I/O burst counts on line " + to_string(lineNumber));
        }


        processList.push_back(currentProcessData);
    }

    inputFile.close();

    if (processList.empty() && lineNumber > 0) {
         cerr << "Warning: File processed, but no valid process definitions were loaded." << endl;                                                  //Error: no valid processes
    } else if (processList.empty() && lineNumber == 0) {
         cerr << "Warning: Input file appears to be empty or only contains comments/blank lines." << endl;                                          //Error: File is empty
    }


    return processList;
}