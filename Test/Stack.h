#include "Node.h"
#include <cstddef>

#ifndef STACK_H_
#define STACK_H_

template <class T>
class Stack {

    public:
        ~Stack();
        void push(T element); // top will have no left pointer
        T pop();
        T peek();

    private:
        Node<T>* top = NULL;

};

#endif

template <class T>
Stack<T>::~Stack() {
    while (peek()) delete pop(); // while not empty clear first element
}

template <class T>
void Stack<T>::push(T element) {
    // stack elements are stored in Nodes of type T
    Node<T>* newElement = new Node<T>(element);
    newElement->setRight(top); // top becomes second element
    top = newElement; // adjust top pointer
}

template <class T>
T Stack<T>::pop() {
    if (top) { // if not empty
        Node<T>* popped = top; // pop top element
        top = popped->getRight(); // set top to next element
        return popped->getValue(); // return T in popped node
        // no need to destruct popped Node because T is returned
        // calling function must destruct T
    } else return NULL;
}

template <class T>
T Stack<T>::peek() {
    if (top) return top->getValue(); // if not empty return top of stack
    else return NULL;
}
