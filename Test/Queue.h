#include "Node.h"
#include <cstddef>

#ifndef QUEUE_H_
#define QUEUE_H_

// reverse stack
template <class T>
class Queue {

    public:
        ~Queue();
        void push(T element); // top will have no left pointer
        T pop();
        T peek();

    private:
        Node<T>* top = NULL;
        Node<T>* bottom = NULL; // new element gets added to bottom
        // since queue is first in first out

};

#endif

template <class T>
Queue<T>::~Queue() {
    while (peek()) delete pop(); // while not empty clear first element
}

template <class T>
void Queue<T>::push(T element) { // only difference from stack
    Node<T>* newElement = new Node<T>(element);
    if (top) { // if not empty, add element to bottom of queue
        bottom->setRight(newElement);
        bottom = newElement;
    } else { // else if empty set top and bottom to new element
        top = newElement;
        bottom = newElement;
    }
}

template<class T>
T Queue<T>::pop() {
    if (top) { // if not empty
        Node<T>* popped = top; // pop top element
        top = popped->getRight(); // set top to next element
        return popped->getValue(); // return T in popped node
        // no need to destruct popped Node because T is returned
        // calling function must destruct T
    } else return NULL;
}

template <class T>
T Queue<T>::peek() {
    if (top) return top->getValue();
    else return NULL;
}
