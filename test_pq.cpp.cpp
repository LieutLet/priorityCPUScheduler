// main.cpp
#include <iostream>
#include <string>
#include <stdexcept> // For catching exceptions
#include <vector>    // For testing with more complex data if needed

// Include the PriorityQueue header file
#include "PriorityQueue.h"

using namespace std;

// Helper function to print status (reduces repetition)
template <typename T>
void print_queue_status(const PriorityQueue<T>& pq, const string& label) {
    cout << "\n--- Status: " << label << " ---" << endl;
    cout << "Is Empty? " << (pq.is_empty() ? "Yes" : "No") << endl;
    cout << "Size: " << pq.size() << endl;
    if (!pq.is_empty()) {
        try {
            cout << "Peek (Highest Prio): " << pq.peek() << endl;
        } catch (const out_of_range& e) {
            cout << "Peek failed (unexpectedly): " << e.what() << endl;
        }
    } else {
        cout << "Peek: N/A (Queue is empty)" << endl;
    }
    // Print detailed content using toString()
    cout << pq.toString() << endl;
    cout << "-------------------------" << endl;
}

int main() {
    cout << "===== Testing PriorityQueue Component =====" << endl;

    
    cout << "\n===== Testing with Integers =====" << endl;
    PriorityQueue<int> pq_int;

    print_queue_status(pq_int, "Initial State");

    cout << "\n>>> Testing dequeue/peek on empty queue..." << endl;
    try {
        pq_int.dequeue();
    } catch (const out_of_range& e) {
        cout << "Caught expected exception on dequeue: " << e.what() << endl;
    }
    try {
        pq_int.peek();
    } catch (const out_of_range& e) {
        cout << "Caught expected exception on peek: " << e.what() << endl;
    }

    cout << "\n>>> Enqueuing items..." << endl;
    pq_int.enqueue(10, 1); // Low priority
    pq_int.enqueue(50, 5); // High priority
    pq_int.enqueue(30, 3); // Mid priority
    pq_int.enqueue(51, 5); // Same high priority (should be behind 50)
    pq_int.enqueue(20, 2); // Low-mid priority
    pq_int.enqueue(52, 5); // Same high priority (should be behind 51)
    pq_int.enqueue(11, 1); // Same low priority (should be behind 10)

    print_queue_status(pq_int, "After Enqueuing Multiple Items");

    cout << "\n>>> Dequeuing items (expecting highest priority first, FIFO within priority)..." << endl;
    while (!pq_int.is_empty()) {
        try {
            cout << "Peeking: " << pq_int.peek() << endl;
            int item = pq_int.dequeue();
            cout << "Dequeued: " << item << " (Size left: " << pq_int.size() << ")" << endl;
        } catch (const out_of_range& e) {
            cout << "Dequeue/Peek failed unexpectedly: " << e.what() << endl;
            break; // Stop if something went wrong
        }
    }

    print_queue_status(pq_int, "After Dequeuing All Items");

    cout << "\n>>> Testing clear()..." << endl;
    pq_int.enqueue(99, 9);
    pq_int.enqueue(1, 0);
    print_queue_status(pq_int, "Before Clear");
    pq_int.clear();
    print_queue_status(pq_int, "After Clear");

     cout << "\n>>> Testing dequeue/peek on cleared queue..." << endl;
    try {
        pq_int.dequeue();
    } catch (const out_of_range& e) {
        cout << "Caught expected exception on dequeue: " << e.what() << endl;
    }
     try {
        pq_int.peek();
    } catch (const out_of_range& e) {
        cout << "Caught expected exception on peek: " << e.what() << endl;
    }


    // --- Test with strings (also tests move semantics implicitly/explicitly) ---
    cout << "\n\n===== Testing with Strings =====" << endl;
    PriorityQueue<string> pq_str;

    print_queue_status(pq_str, "Initial State");

    cout << "\n>>> Enqueuing strings (const T& version)..." << endl;
    string task1 = "CRITICAL Task";
    string task2 = "Low Priority Task";
    string task3 = "Medium Task A";
    string task4 = "Medium Task B";

    pq_str.enqueue(task1, 0);
    pq_str.enqueue(task2, 10);
    pq_str.enqueue(task3, 5);
    pq_str.enqueue(task4, 5); // Same priority as task3

    print_queue_status(pq_str, "After Enqueuing Const Refs");

    cout << "\n>>> Enqueuing strings (T&& move version)..." << endl;
    string temp_task_high = "Another CRITICAL";
    string temp_task_low = "Very Low";
    pq_str.enqueue(move(temp_task_high), 0); // Should go after "CRITICAL Task"
    pq_str.enqueue(move(temp_task_low), 10);

    // After moving, the original strings are in a valid but unspecified state
    // cout << "Original temp_task_high after move: '" << temp_task_high << "'" << endl; // Behavior depends on std::string impl.

    print_queue_status(pq_str, "After Enqueuing Moved Strings");


    cout << "\n>>> Dequeuing strings..." << endl;
    while (!pq_str.is_empty()) {
         try {
            cout << "Peeking: \"" << pq_str.peek() << "\"" << endl;
            string item = pq_str.dequeue();
            cout << "Dequeued: \"" << item << "\" (Size left: " << pq_str.size() << ")" << endl;
        } catch (const out_of_range& e) {
            cout << "Dequeue/Peek failed unexpectedly: " << e.what() << endl;
            break; // Stop if something went wrong
        }
    }

    print_queue_status(pq_str, "After Dequeuing All Strings");


    cout << "\n===== Priority Queue Tests Complete =====" << endl;

    return 0;
}