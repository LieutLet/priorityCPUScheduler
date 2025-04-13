//*********************************************************************//
// A data structure to keep track of the information about the process //
// Contains the following information:								   //
//		1. A stack pointer to the address of the process stack         //
//		2. Process state (running, ready, waiting)                     //
//		3. Process ID (PID)                                            //
//		4. Memory limits											   //
//		5. A list of pointers to the open files for the process        //
//*********************************************************************//

//#include <unistd.h>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

#ifndef PCB
#define PCB

class PCB
{
public:
	// Constructor
	PCB(int pid, int stackPointer);

	// Getters
	int getPID() const { return pid; }
	int getStackPointer() const { return stackPointer; }
	int getState() const { return state; }
	int getOpenFiles() const { return openFiles; }
	//int getRunTime() const { return runTime; }
	// Setters
	void setMemoryLimit(int limit) { memoryLimit = limit; }
	void setState(int newState) { state = newState; }
	int addOpenFiles(string file);
	//void setRunTime(int time) { runTime = time; }

	// Other methods
	int removeOpenFiles(string file);		// Remove file from open files

private:
	int pid;								// Process ID
	int stackPointer;						// Stack pointer
	int state;								// Process state (running, ready, waiting)
	int memoryLimit;						// Memory limit
	vector<string>openFiles;				// List of open files
	//int runTime;							// Run time

};

//Default constructor
PCB::PCB()
{
	pid = 0;
	stackPointer = 0;
	state = 0;								// Default state
	memoryLimit = 0;						// Default memory limit
	//runTime = 0;							// Default run time
}

// Constructor
PCB::PCB(int pid, int stackPointer)
{
	pid = pid;
	stackPointer = stackPointer;
	state = 0;								// Default state
	memoryLimit = 0;						// Default memory limit
	//runTime = 0;							// Default run time
}

// *****************************************************************//
// Function to add files to the list of open files.				    //
// Takes the pathname of the file as a parameter.					//
// If there is space in the list, the file is added, and 1 is		//
// returned. If there is no space, 0 is returned.					//
// *****************************************************************//
int PCB::addOpenFiles(string file);
{
	if (openFiles.size() <= memoryLimit) {
		openFiles.append(file);					// Add file to open files
		return 1;								// File added successfully
	}
	else {
		perror("Error: Memory limit exceeded");
		return 0;								// File not added
	}
}

// *****************************************************************//
// A function to remove a specific file from the list of open files //
// returns 0 if the file could not be removed, 1 if it was.		    //
// *****************************************************************//
int PCB::removeOpenFiles(string file);
{
	if (openFiles.empty() == true) {
		perror("Error: No open files");
		return 0;
	}
	for (int i = 0; i < openFiles.size(); i++) {
		if (openFiles[i] == file) {
			openFiles.erase(openFiles.begin() + i);	// Remove file from open files
			return 1;
		}
		else {
			perror("Error: File not found");
			return 0;
		}
	}
}
#endif // !PCB