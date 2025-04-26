#include "PCB.h" 
#include <iostream> 
#include <vector>   
#include <string>  
using namespace std;

int main() {
    // --- Test Case 1: Basic Process Creation ---
    cout << "--- Test Case 1: Basic Process Creation ---" << endl;

    // Create a new process with ID 1, priority 5, and arrival time 0
    Process p1(1, 5, 0);

    // Verify initial static characteristics
    cout << "Process ID: " << p1.processID << endl;
    cout << "Priority: " << p1.priority << endl;
    cout << "Arrival Time: " << p1.arrivalTime << endl;

    // Verify initial dynamic state (should be NEW)
    if (p1.currentState == State::NEW) {
        cout << "Initial State: NEW" << endl;
    } else {
        cout << "Initial State: Other (Expected NEW)" << endl;
    }
    cout << "Initial Current Burst Index: " << p1.currentBurstIndex << endl;
    cout << "Initial Time Remaining in Burst: " << p1.timeRemainingInBurst << endl;

    // Verify initial statistics (should be default values)
    cout << "Initial Start Time: " << p1.startTime << endl;
    cout << "Initial Completion Time: " << p1.completionTime << endl;
    cout << "Initial Turnaround Time: " << p1.turnaroundTime << endl;
    cout << "Initial Waiting Time: " << p1.waitingTime << endl;
    cout << "Initial Response Time: " << p1.responseTime << endl;
    cout << "Initial CPU Time Executed: " << p1.cpuTimeExecuted << endl;
    cout << "Initial I/O Time: " << p1.ioTime << endl;

    cout << endl; 


    // --- Test Case 2: Adding Bursts and Modifying State ---
    cout << "--- Test Case 2: Adding Bursts and Modifying State ---" << endl;

    // Add some CPU and I/O bursts to the process
    p1.cpuBursts.push_back(10); // First CPU burst = 10 time units
    p1.ioBursts.push_back(5);  // First I/O burst = 5 time units
    p1.cpuBursts.push_back(8);  // Second CPU burst = 8 time units
    p1.ioBursts.push_back(4);  // Second I/O burst = 4 time units

    cout << "CPU Bursts added: ";
    for (int burst : p1.cpuBursts) {
        cout << burst << " ";
    }
    cout << endl;

    cout << "I/O Bursts added: ";
    for (int burst : p1.ioBursts) {
        cout << burst << " ";
    }
    cout << endl;

    // Simulate a state change (process moves to READY)
    p1.currentState = State::READY;
    if (p1.currentState == State::READY) {
        cout << "Updated State: READY" << endl;
    }

    // Simulate starting the first burst
    p1.currentBurstIndex = 0;
    p1.timeRemainingInBurst = p1.cpuBursts[p1.currentBurstIndex];
    p1.currentState = State::RUNNING;
     if (p1.currentState == State::RUNNING) {
        cout << "Updated State: RUNNING" << endl;
    }
    cout << "Current Burst Index: " << p1.currentBurstIndex << endl;
    cout << "Time Remaining in Current Burst: " << p1.timeRemainingInBurst << endl;

    cout << endl;


    // --- Test Case 3: Simulating Time Passing and Updating Statistics ---
    cout << "--- Test Case 3: Simulating Time Passing and Updating Statistics ---" << endl;

    // Simulate 3 time units of execution
    int time_elapsed = 3;
    p1.timeRemainingInBurst -= time_elapsed;
    p1.cpuTimeExecuted += time_elapsed;

    cout << "After " << time_elapsed << " time units of execution:" << endl;
    cout << "Time Remaining in Current Burst: " << p1.timeRemainingInBurst << endl;
    cout << "Total CPU Time Executed: " << p1.cpuTimeExecuted << endl;

    // Simulate the process finishing its current CPU burst and going to WAITING
    if (p1.timeRemainingInBurst == 0) {
        cout << "CPU burst finished. Transitioning to WAITING." << endl;
        p1.currentState = State::WAITING;
        // Move to the corresponding I/O burst
        p1.timeRemainingInBurst = p1.ioBursts[p1.currentBurstIndex];
         if (p1.currentState == State::WAITING) {
            cout << "Updated State: WAITING" << endl;
        }
        cout << "Time Remaining in Current (I/O) Burst: " << p1.timeRemainingInBurst << endl;
    }

    cout << endl;

    // --- Test Case 4: Simulating Completion and Calculating Statistics ---
    cout << "--- Test Case 4: Simulating Completion and Calculating Statistics ---" << endl;

    // Manually set some values to simulate completion
    // In the real simulation, these would be calculated based on events
    p1.completionTime = 25; // Assume completion at time 25
    p1.startTime = 2;       // Assume started running at time 2
    p1.currentState = State::TERMINATED;

    // Calculate some statistics
    p1.turnaroundTime = p1.completionTime - p1.arrivalTime;
    
    // Simmulated calculating turnaround time
    int total_cpu_needed = 0;
    for(int burst : p1.cpuBursts) total_cpu_needed += burst;
    int total_io_needed = 0;
    for(int burst : p1.ioBursts) total_io_needed += burst;
    p1.waitingTime = p1.turnaroundTime - total_cpu_needed - total_io_needed;
    p1.responseTime = p1.startTime - p1.arrivalTime;
     if (p1.currentState == State::TERMINATED) {
        cout << "Updated State: TERMINATED" << endl;
    }

    cout << "Completion Time: " << p1.completionTime << endl;
    cout << "Start Time: " << p1.startTime << endl;
    cout << "Calculated Turnaround Time: " << p1.turnaroundTime << endl;
    cout << "Calculated Waiting Time (Simplified): " << p1.waitingTime << endl;
    cout << "Calculated Response Time: " << p1.responseTime << endl;


    return 0; 
}
