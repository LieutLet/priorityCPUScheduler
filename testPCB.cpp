#include<iostream>
#include<string>
#include<cstdio>
#include<vector>

#include "PCB.h"

using namespace std;

// *******************************************
// This is just to test the PCB class
// *******************************************
int main()
{
	PCB process1(1, 1000);
	vector<string> files;

	cout << "Default Process Information:" << endl;

	cout << "Process ID: " << process1.getPID() << endl;
	cout << "Stack Pointer: " << process1.getStackPointer() << endl;
	cout << "State: " << process1.getState() << endl;
	cout << "Memory Limit: " << process1.getMemoryLimit() << endl;
	cout << "Open Files: " << process1.getOpenFiles(files) << endl;

	cout << " ******************************************" << endl;
	cout << "updating process information..." << endl;

	process1.setMemoryLimit(2000);
	process1.setState(1);
	process1.addOpenFiles("file1.txt");
	process1.addOpenFiles("file2.txt");

	cout << "Process ID: " << process1.getPID() << endl;
	cout << "Stack Pointer: " << process1.getStackPointer() << endl;
	cout << "State: " << process1.getState() << endl;
	cout << "Memory Limit: " << process1.getMemoryLimit() << endl;
	cout << "Open Files: " << process1.getOpenFiles(files) << endl << endl;


	return 0;
}
