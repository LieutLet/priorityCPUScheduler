#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <map>                                                                                                             
#include <queue>                                                                                
#include <stdexcept>  // For exceptions (e.g., dequeue from empty)
#include <vector>     // Useful for potential debugging/toString methods
#include <sstream>    // For toString method


using namespace std;

// ********* Priority Convention: Lower integer value means higher priority *********************************

template <typename T>
class PriorityQueue {
private:
    // Map from priority (int) to a queue of items (T) with that priority
    // map keeps keys sorted, so the lowest integer key is always first
    map<int, queue<T>> queues;

    // Track total size for O(1) size() operation
    size_t total_size = 0;

public:
    // Default constructor: initializes an empty priority queue
    PriorityQueue() = default;

   
    virtual ~PriorityQueue() = default;

   

    /**
     * Description: Adds an item to the queue with a given priority. Items of the same priority are processed FIFO
     *              This version handles const data.
     *
     * Parameters:
     *      item: The item to add to the queue.
     *      priority: The priority level (lower value means higher priority).
     */
    void enqueue(const T& item, int priority) {
        // Access the queue for the given priority.
        // If the priority key doesn't exist, map automatically creates
        // a new queue<T> for it using its default constructor.
        queues[priority].push(item);
        total_size++;
    }

    /**
     * //Desicription: Adds an item to the queue with a given priority by moving it for better efficiency
     *                 
     *
     * Parameters: 
     *      item: The item to add to the queue. 
     *      priority: The priority level (lower value means higher priority).
     */
    void enqueue(T&& item, int priority) {
        queues[priority].push(move(item));
        total_size++;
    }


    /**
     * Description: Removes and returns the highest priority item from the queue.
     *              If multiple items share the highest priority, the one added enqueued
     *              (FIFO) is returned.
     *
     * Return: The highest priority item (by value).
     * Throws: out_of_range If the queue is empty.
     */
    T dequeue() {
        if (is_empty()) {
            throw out_of_range("Dequeue called on an empty PriorityQueue");
        }

        // Get an iterator to the first element in the map (highest priority)
        auto highest_prio_it = queues.begin(); // O(1)

        // Get the priority value and a reference to the associated queue
        int highest_priority = highest_prio_it->first;
        queue<T>& highest_queue = highest_prio_it->second;

        // Get the item from the front of the highest priority queue
        // Use move if possible, otherwise copy.
        T item = move(highest_queue.front()); // O(1)

        // Remove the item from the front of that queue
        highest_queue.pop(); // O(1)
        total_size--;

        // If removing the item made the priority queue empty, remove its entry from the map
        if (highest_queue.empty()) {
            
            queues.erase(highest_priority); // O(log N) where N = # priorities
        }

        return item;
    }

    /**
     * Description: Returns a const reference to the highest priority item without removing it.
     *              If multiple items share the same priority, return a reference to the first item enqueued
     *
     * Return: A constant reference to the highest priority item.
     * Throws: out_of_range If the queue is empty.

     * Warnings: The returned reference is only valid until the next non-constant
     *           operation (enqueue/dequeue) modifies the queues state.
     */
    const T& peek() const {
        if (is_empty()) {
            throw out_of_range("Peek called on an empty PriorityQueue");
        }

        // Get a const iterator to the first map element (highest priority)
        auto highest_prio_it = queues.begin(); // O(1)

        // Get a const reference to the associated queue
        const queue<T>& highest_queue = highest_prio_it->second;

        // Return a const reference to the front item
        return highest_queue.front(); // O(1)
    }

    /**
     * Descripton: Checks if the priority queue is empty.
     *
     * Return: True if the queue contains no items, false otherwise.
     */
    bool is_empty() const {
        // return queues.empty(); // Also works
        return total_size == 0;
    }

    /**
     * Description: Gets the total number of items in the priority queue.
     *
     * Return: The total number of items across all priority levels.
     */
    size_t size() const {
        return total_size;
    }

    /**
     * Description: Removes all items from the priority queue.
     */
    void clear() {
        queues.clear();
        total_size = 0;
    }

    /**
     * Description: Provides a string representation of the queue contents (for debugging).
     * 
     * Return: A string describing the queue state.
     */
    string toString() const {
         if (is_empty()) {
            return "PriorityQueue: Is empty";
        }

        stringstream ss;
        ss << "PriorityQueue (Highest Priority First):\n";

        // Iterate through the map in order (highest priority first)
        for (auto it = queues.begin(); it != queues.end(); ++it) {
            int priority = it->first;
            const queue<T>& q_ref = it->second;

            // Create a temporary copy to iterate without modifying the original
            queue<T> temp_q = q_ref;

            ss << "  Priority " << priority << ": [";
            bool first = true;
            while (!temp_q.empty()) {
                 if (!first) {
                    ss << ", ";
                }
                 
                 try {
                     ss << temp_q.front();
                 } catch (...) {
                      ss << "<non-streamable item>"; // Fallback
                 }

                temp_q.pop();
                first = false;
            }
            ss << "]\n";
        }
        ss << "Total items: " << size();
        return ss.str();
    }
};

#endif // PRIORITY_QUEUE_H