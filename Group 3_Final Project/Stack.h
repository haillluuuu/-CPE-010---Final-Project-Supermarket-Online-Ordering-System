#ifndef STACK_H
#define STACK_H


#include <iostream>

template <typename T>
struct StackNode {
    T data;
    StackNode<T>* next = nullptr;
};

template <typename T>
class Stack {
private:
    StackNode<T>* topNode = nullptr;

public:
    ~Stack() {
        clear();
    }

    // Pushes a new element onto the top of the stack
    void push(T value) {
        StackNode<T>* newNode = new StackNode<T>;
        newNode->data = value;
        newNode->next = topNode;
        topNode = newNode;
    }

    // Removes and returns the top element from the stack
    T pop() {
        if (isEmpty()) {
            std::cout << "Stack Underflow!\n";
            return T();
        }
        StackNode<T>* temp = topNode;
        T value = temp->data;
        topNode = temp->next;
        delete temp;
        return value;
    }

    // Returns data stored at the top node without removing it
    T top() const {
        if (isEmpty()) {
            std::cout << "Stack is empty!\n";
            return T();
        }
        return topNode->data;
    }

    // Checks if the stack is currently empty
    bool isEmpty() const {
        return topNode == nullptr;
    }

    // Clears all elements stored in the stack
    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }

    // Displays all items currently present in the stack
    void displayAll() const {
        if (isEmpty()) {
            std::cout << "Stack is empty\n";
            return;
        }
        std::cout << "Stack contents:\n";
        StackNode<T>* temp = topNode;
        while (temp != nullptr) {
            std::cout << " | " << temp->data << "\n";
            temp = temp->next;
        }
    }
};

#endif