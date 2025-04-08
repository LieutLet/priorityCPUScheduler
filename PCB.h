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

using namespace std;

#ifndef PCB
#define PCB

class PCB
{
public:
	// Constructor
	PCB(int pid, int stackPointer);
	// Destructor
	~PCB();
	// Getters
	int getPID() const { return pid; }
	int getStackPointer() const { return stackPointer; }
	int getState() const { return state; }
	int getOpenFiles() const { return openFiles; }
	// Setters
	void setMemoryLimit(int limit) { memoryLimit = limit; }
	void setState(int state) { state = newState; }
	void setOpenFiles(int* openFiles) { this->openFiles = openFiles; }

private:
	int pid;								// Process ID
	int stackPointer;						// Stack pointer
	int state;								// Process state (running, ready, waiting)
	int memoryLimit;						// Memory limit
	int* openFiles[memoryLimit];			// List of open files

}

#endif // !PCB