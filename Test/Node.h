#include <cstddef>

#ifndef NODE_H_
#define NODE_H_

template <class T>
class Node {

    public:
        Node(T value);
        ~Node();
        // setters
        void setValue(T value);
        void setLeft(Node* left); // used in linked list type data structure
        void setRight(Node* right); // used in linked list type data structure
        // getters
        T getValue();
        Node<T>* getLeft();
        Node<T>* getRight();

    private:
        T value;
        Node<T>* left;
        Node<T>* right;

};

template <class T>
Node<T>::Node(T value) {
    left = right = NULL;
    this->value = value;
}

template <class T>
Node<T>::~Node() {
    delete value;
}

template <class T>
void Node<T>::setValue(T value) {
    this->value = value;
}

template <class T>
void Node<T>::setLeft(Node<T>* left) {
    this->left = left;
}

template <class T>
void Node<T>::setRight(Node<T>* right) {
    this->right = right;
}

template <class T>
T Node<T>::getValue() {
    return value;
}

template <class T>
Node<T>* Node<T>::getLeft() {
    return left;
}

template <class T>
Node<T>* Node<T>::getRight() {
    return right;
}

#endif
