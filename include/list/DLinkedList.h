/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; 
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }

        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
        
    class BWDIterator {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool last = true) {
            if (last) {
                if (pList != 0)
                    this->pNode = pList->tail->prev; // Start before tail (to skip dummy tail node)
                else
                    pNode = 0;
            } else {
                if (pList != 0)
                    this->pNode = pList->head; // End at the head
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &iterator) {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->next; 
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

   
        T &operator*() {
            return pNode->data;
        }

        bool operator!=(const BWDIterator &iterator) {
            return pNode != iterator.pNode;
        }

        // Prefix -- overload
        BWDIterator &operator++() {
            if (pNode != nullptr && pNode->prev != nullptr) {
                pNode = pNode->prev;
            }
            return *this;
        }

        // Postfix -- overload
        BWDIterator operator++(int) {
            BWDIterator iterator = *this;
            ++*this;
            return iterator;
        }

        // // Prefix ++ overload 
        // BWDIterator &operator++() {
        //     if (pNode != nullptr && pNode->next != nullptr) {
        //         pNode = pNode->next;
        //     }
        //     return *this;
        // }

        // // Postfix ++ overload (move forward, but return original iterator)
        // BWDIterator operator++(int) {
        //     BWDIterator iterator = *this;
        //     ++*this;
        //     return iterator;
        // }

    };

};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    // cout << "Constructor done\n";
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;

}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;

    Node* tmp = list.head->next;
    while(tmp != list.tail)
    {
        add(tmp->data);
        tmp = tmp->next;
    }
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
    if(this == &list)
        return *this;
    clear();
    delete head;
    delete tail;
    head = tail = new Node();
    head->next = tail;
    tail->prev = head;
    count = 0;

    Node* tmp = list.head->next;
    while(tmp != list.tail)
    {
        add(tmp->data);
        tmp = tmp->next;
    }

    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    // if(deleteUserData)
    //     deleteUserData(this);
    clear();
    delete head;
    delete tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    Node *newNode = new Node(e, tail, tail->prev);
    tail->prev = tail->prev->next = newNode;
    
    count+=1;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    Node* tmp = head;
    int i = 0;

    if(index < 0 || index > count)
        throw std::out_of_range("Index is out of range!");

    while(i <= index && tmp != nullptr)
    {
        tmp = tmp->next;
        i++;
    }
    tmp->prev = tmp->prev->next = new Node(e, tmp, tmp->prev);
    count+=1;
    // delete tmp;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    // TODO
    if (index <= 0 || index > count) return nullptr;

    Node* tmp;
    if(index <= count/2)
    {
        tmp = head;
        for(size_t i = 0; i < index; i++)
            tmp = tmp->next;
    }
    else
    {
        tmp = tail;
        for(size_t i = count; i > index; i--)
            tmp = tmp->prev;
    }

    return tmp;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // TODO
    if(index < 0 || index >= count || count == 0)
        throw std::out_of_range("Index is out of range!");
    
    Node* target = head->next;
    int i = 0;
    while(i < index && target != tail)
    {
        i++;
        target = target->next;
    }

    T val = target->data;

    target->prev->next = target->next;
    target->next->prev = target->prev;

    delete target;
    count--;
    return val; 

}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    if(this->count == 0)
        return true;
    else
        return false;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    if(head == nullptr || tail == nullptr)
        return;

    // if(deleteUserData)
    //     deleteUserData(this);
    
    Node* tmp = head->next;
    while(tmp != tail)
    {
        Node* toDelete = tmp;
        tmp = tmp->next;
        // if(deleteUserData)
        //     deleteUserData(toDelete);
        
        delete toDelete;
    }
    delete head;
    delete tail;
    // head->next = tail;
    // tail->prev = head;
    head = new Node();
    tail = new Node();
    head->next = tail;
    tail->prev = head;
    count = 0;
    // delete tmp;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    Node* tmp = head;
    int i = 0;

    if(index < 0 || index >= count || count == 0)
        throw std::out_of_range("Index is out of range!");

    while(i <= index && tmp != nullptr)
    {
        i++;
        tmp = tmp->next;
    }
    return tmp->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    Node* tmp = head->next;
    int i = 0;
    
    while(tmp != tail)
    {
        // if(itemEqual != nullptr)
        //     if(itemEqual(tmp->data, item, itemEqual))
        //         return i;
        // else
        // if(tmp->data == item)
        if(equals(tmp->data, item, itemEqual))
            return i;
        i++;
        tmp = tmp->next;
    }
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // Removes the first element in the list with a value equal to item.
    // TODO
    Node *tmp = head->next;
    while (tmp != tail)
    {   
        if(equals(tmp->data, item, itemEqual))
        {
            if(removeItemData != nullptr)
                removeItemData(tmp->data);

            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;

            delete tmp;

            count--;
            
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    Node* tmp = head->next;
    while (tmp != tail)
    {
        if (itemEqual != nullptr)
        {
            if (itemEqual(tmp->data, item))
                return true;
        }
        else
        {
            if (tmp->data == item) 
                return true;
        }

        tmp = tmp->next;
    }
    return false;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    if(count == 0)
        return "[]";

    std::stringstream os;
    os << "[";
    bool hasItem2str = (item2str != nullptr);
    Node *tmp = head->next;

    while(tmp != tail && tmp != nullptr)
    {
        if(tmp != head->next)
            os << ", ";
        if (hasItem2str)
            os << item2str(tmp->data);
        else 
            os << tmp->data;
        
        tmp = tmp->next;
    }

    os << "]";
    return os.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // TODO
    clear();
    itemEqual = list.itemEqual;
    deleteUserData = list.deleteUserData;
    Node* tmp = list.head->next;
    while(tmp != list.tail)
    {
        add(tmp->data);
        tmp = tmp->next;
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // TODO
    Node* tmp = head->next;
    while(tmp != tail)
    {
        Node* nextNode = tmp->next;
        if(deleteUserData!=nullptr)
            deleteUserData(&tmp->data);
        delete tmp;
        tmp = nextNode;
    }
    head->next = tail;
    tail->prev = head;
    count = 0;
}

#endif /* DLINKEDLIST_H */
