//*********************************************************************//
// A data structure to keep track of the information about the process //
// Contains the following information:				       //
//		1. A stack pointer to the address of the process stack //
//		2. Process state (running, ready, waiting)             //
//		3. Process ID (PID)                                    //
//		4. Memory limits				       //
//		5. A list of pointers to the open files for the process//
//*********************************************************************//

//#include <unistd.h>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

#ifndef PCB_H
#define PCB_H

class PCB
{
public:
	// Default constructor
	PCB();

	// Constructor
	PCB(int p, int sP);

	// Getters

	// **********************************************************//
	// Returns the process ID.				     //
	// **********************************************************//
	int getPID() const { return pid; }

	// **********************************************************//
	// Returns the stack pointer.				     //
	// **********************************************************//
	int getStackPointer() const { return stackPointer; }

	// **********************************************************//
	// Returns the state of the process.			     //
	// **********************************************************//
	int getState() const { return state; }

	int getOpenFiles(vector<string>& files) const;

	// **********************************************************//
	// Returns the memory limit of the process.		     //
	// **********************************************************//
	int getMemoryLimit() const { return memoryLimit; }

	// **********************************************************//
	// Temp function to return the run time of the process.	     //
	// **********************************************************//
	//int getRunTime() const { return runTime; }

	// Setters

	// **********************************************************//
	// Sets the memory limit of the process.		     //
	// **********************************************************//
	void setMemoryLimit(int limit) { memoryLimit = limit; }

	// **********************************************************//
	// changes the state of the process.			     //
	// **********************************************************//
	void setState(int newState) { state = newState; }

	// **********************************************************//
	// function to add a file to the list of open files, returns //
	// 0 adding that file excedes the memory limit.	             //
	// **********************************************************//
	int addOpenFiles(string file);

	// **********************************************************//
	// temp function to change the runtime of the process.	     //
	// **********************************************************//
	//void setRunTime(int time) { runTime = time; }

	// **********************************************************//
	// function to remove a file from the list of open files.    //
	// returns 0 if the file could not be found, 1 if it was.    //
	// **********************************************************//
	int removeOpenFiles(string file);				// Remove file from open files

private:
	int pid;							// Process ID
	int stackPointer;						// Stack pointer
	int state;							// Process state (running, ready, waiting)
	int memoryLimit;						// Memory limit
	vector<string>openFiles;					// List of open files
	//int runTime;							// Run time

};

//Default constructor
PCB::PCB()
{
	pid = 0;
	stackPointer = 0;
	state = 0;							// Default state
	memoryLimit = 0;						// Default memory limit
	//runTime = 0;							// Default run time
}

// Constructor
PCB::PCB(int p, int sP)
{
	pid = p;
	stackPointer = sP;
	state = 0;							// Default state
	memoryLimit = 0;						// Default memory limit
	//runTime = 0;							// Default run time
}

// *****************************************************************//
// Function to add files to the list of open files.		    //
// Takes the pathname of the file as a parameter.		    //
// If there is space in the list, the file is added, and 1 is	    //
// returned. If there is no space, 0 is returned.		    //
// *****************************************************************//
int PCB::addOpenFiles(string file)
{
	if (openFiles.size() <= memoryLimit) {
		openFiles.push_back(file);				// Add file to open files
		return 1;						// File added successfully
	}
	else {
		perror("Error: Memory limit exceeded");
		return 0;						// File not added
	}
	return 0;							// File not added
}

// *****************************************************************//
// A function to remove a specific file from the list of open files //
// returns 0 if the file could not be removed, 1 if it was.	    //
// *****************************************************************//
int PCB::removeOpenFiles(string file)
{
	if (openFiles.empty() == true) {
		perror("Error: No open files");
		return 0;
	}
	for (int i = 0; i < openFiles.size(); i++) {
		if (openFiles[i] == file) {
			openFiles.erase(openFiles.begin() + i);		// Remove file from open files
			return 1;
		}
		else {
			perror("Error: File not found");
			return 0;
		}
	}
	return 0;							// File not found
}

// *****************************************************************//
// A function to get the list of open files.			    //
// returns 0 if there are no open files, 1 if there are.	    //
// *****************************************************************//
int PCB::getOpenFiles(vector<string>& files) const
{
	if (openFiles.empty() == true) {
		perror("Error: No open files");
		return 0;
	}

	files = openFiles;						// Copy open files to the vector
	return 1;
}
#endif // !PCB
