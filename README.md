# priorityCPUSchedular
CSC-377 Semester Project. It is a CPU scheduler simulation that uses a priority queue to schedule processes on a "CPU".

If you want to make any changes to the file, run "g++ os.cpp ProcessLoader.cpp -o os_simulation -std=c++14" to compile

If the os_simulation.exe doesn't work, you will need to compile it using the above command. 

To change which processes to test the program with, you will need to edit the "processes.txt" file, this is the easiest method of editing the processes. The more complicated way is to edit os.cpp, and change the string filename to the preferred file (around line 83 in the main function). 

testPCB.cpp, test_pq.cpp are both special test files created to test PCB.h and priorityQueue.h they are not needed to compile the program.
