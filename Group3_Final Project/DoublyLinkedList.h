#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <iostream>

template <typename T>
class DoubleList {
public:
    T data;
    DoubleList<T>* next = nullptr;
    DoubleList<T>* prev = nullptr;
};

// Dynamically allocates memory for a new node storing newData
template <typename T>
DoubleList<T>* new_Node(T newData) {
    DoubleList<T>* newNode = new DoubleList<T>;
    newNode->data = newData;
    newNode->next = nullptr;
    newNode->prev = nullptr;
    return newNode;
}

// Traverses and displays list elements both forward and backward
template <typename T>
void DllTraverse(DoubleList<T>* currentNode) {
    DoubleList<T>* tail = nullptr;

    std::cout << "Forward Traversal:\n";
    while (currentNode != nullptr) {
        std::cout << currentNode->data << " ";
        tail = currentNode;
        currentNode = currentNode->next;
    }
    std::cout << "\n";

    std::cout << "Reverse Traversal:\n";
    while (tail != nullptr) {
        std::cout << tail->data << " ";
        tail = tail->prev;
    }
    std::cout << "\n\n";
}

// Inserts a new node at the head of the list
template <typename T>
void DllInsertHead(T newData, DoubleList<T>*& currenthead) {
    DoubleList<T>* newNode = new_Node(newData);

    newNode->next = currenthead;
    newNode->prev = nullptr;
    if (currenthead != nullptr) {
        currenthead->prev = newNode;
    }
    currenthead = newNode;
}

// Inserts a new node immediately after prevNode
template <typename T>
void DllgeneralInsert(T newData, DoubleList<T>* prevNode) {
    if (prevNode == nullptr) {
        std::cout << "Previous node cannot be null.\n";
        return;
    }

    DoubleList<T>* newNode = new_Node(newData);

    newNode->next = prevNode->next;
    newNode->prev = prevNode;

    if (prevNode->next != nullptr)
        prevNode->next->prev = newNode;

    prevNode->next = newNode;
}

// Inserts a new node at the end (tail) of the list
template <typename T>
void DllInsertEnd(T newData, DoubleList<T>*& currenthead) {
    DoubleList<T>* newNode = new_Node(newData);

    if (currenthead == nullptr) {
        currenthead = newNode;
        return;
    }

    DoubleList<T>* tail = currenthead;
    while (tail->next != nullptr) {
        tail = tail->next;
    }

    tail->next = newNode;
    newNode->prev = tail;
}

// Deletes the first node whose data equals findData
template <typename T>
void DllDelete(T findData, DoubleList<T>*& head) {
    if (head == nullptr)
        return;

    DoubleList<T>* currNode = head;

    while (currNode != nullptr && !(currNode->data == findData))
        currNode = currNode->next;

    if (currNode == nullptr)
        return;

    if (currNode->prev != nullptr)
        currNode->prev->next = currNode->next;
    else
        head = currNode->next;

    if (currNode->next != nullptr)
        currNode->next->prev = currNode->prev;

    delete currNode;
}

// Deletes the entire list and frees memory, resetting head pointer to nullptr
template <typename T>
void DllDeleteList(DoubleList<T>*& head) {
    DoubleList<T>* currNode = head;

    while (currNode != nullptr) {
        DoubleList<T>* temp = currNode;
        currNode = currNode->next;
        delete temp;
    }
    head = nullptr;
}

// Searches and returns the node matching findData, or nullptr if not found
template <typename T>
DoubleList<T>* DllFind(T findData, DoubleList<T>* head) {
    while (head != nullptr) {
        if (head->data == findData)
            return head;
        head = head->next;
    }
    return nullptr;
}

// Returns the total number of nodes in the list
template <typename T>
int DllCount(DoubleList<T>* head) {
    int count = 0;
    while (head != nullptr) {
        count++;
        head = head->next;
    }
    return count;
}

#endif