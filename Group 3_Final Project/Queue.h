#ifndef QUEUE_H
#define QUEUE_H

#include "DoublyLinkedList.h"

template <typename T>
struct Queue {
    DoubleList<T>* front = nullptr;
    DoubleList<T>* rear = nullptr;
    int count = 0;
};

// Returns true when there is nothing in the queue
template <typename T>
bool queueIsEmpty(const Queue<T>& q) {
    return q.front == nullptr;
}

// ENQUEUE 
// Add data at the back of the queue 
template <typename T>
void enqueue(Queue<T>& q, T data) {
    DoubleList<T>* newNode = new_Node(data);

    if (q.rear == nullptr) {          // if queue was empty
        q.front = newNode;
        q.rear = newNode;
    } else {
        q.rear->next = newNode;       // else old rear points forward to new node
        newNode->prev = q.rear;       // New node points back to old rear
        q.rear = newNode;
    }
    q.count++;
}

// DEQUEUE
// Removes the node at the FRONT and assigns its data to the removeItem parameter
template <typename T>
bool dequeue(Queue<T>& q, T& out) {
    if (q.front == nullptr)
        return false;

    DoubleList<T>* temp = q.front;
    out = temp->data;

    q.front = temp->next;
    if (q.front != nullptr)
        q.front->prev = nullptr;      // New front has no preceding node
    else
        q.rear = nullptr;             // Queue became empty

    delete temp;
    q.count--;
    return true;
}

// PEEK
// Returns a pointer to data at the front without removing it
template <typename T>
T* peekFront(Queue<T>& q) {
    if (q.front == nullptr)
        return nullptr;
    return &q.front->data;
}

// FIND
// Searches for the node whose data matches target value
template <typename T>
DoubleList<T>* queueFind(Queue<T>& q, T value) {
    return DllFind(value, q.front);
}

// REMOVE FROM MIDDLE
// Unlinks and deletes node matching value (for order cancellation)
template <typename T>
bool removeFromQueue(Queue<T>& q, T value) {
    DoubleList<T>* node = DllFind(value, q.front);
    if (node == nullptr)
        return false;

    if (node->prev != nullptr)
        node->prev->next = node->next;
    else
        q.front = node->next;         // Removed the front node

    if (node->next != nullptr)
        node->next->prev = node->prev;
    else
        q.rear = node->prev;          // Removed the rear node

    delete node;
    q.count--;
    return true;
}

// Empties the queue and frees all node memory
template <typename T>
void clearQueue(Queue<T>& q) {
    DllDeleteList(q.front);
    q.rear = nullptr;
    q.count = 0;
}

#endif