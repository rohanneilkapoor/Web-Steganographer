//
//  HashTable.h
//  Proj 4
//
//  Created by Rohan Kapoor on 3/5/15.
//  Copyright (c) 2015 FBGM. All rights reserved.
//

#ifndef Proj_4_HashTable_h
#define Proj_4_HashTable_h

#include <iostream>
using namespace std;

template <typename KeyType, typename ValueType>
class HashTable
{
public:
    HashTable(unsigned int numBuckets, unsigned int capacity);
    ~HashTable();
    bool isFull() const;
    bool set(const KeyType& key, const ValueType& value, bool permanent = false);
    bool get(const KeyType& key, ValueType& value) const;
    bool touch(const KeyType& key);
    bool discard(KeyType& key, ValueType& value);
private:
    struct Node
    {
        KeyType key;
        ValueType val;
        Node* next; //ptr to the next in the non history linked list
        Node* previous; //ptr to the previous in the non history linked list
        Node* hNext; //ptr to next in the history linked list
        Node* hPrevious; //ptr to the previous in the history linked list
        bool perm = false;
    };
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
    unsigned int getBucketForKey(const KeyType& key) const;
    Node* insertToFront(Node*& ptr,const KeyType& key, const ValueType& value, bool permanent);
    void insertToFrontHist(Node*& ptr,const KeyType& key, const ValueType& value, bool permanent, Node*& passedNode);
    void erase(Node*& ptr);
    Node** bucketList;
    Node* mRecent; //ptr to the most recent element
    Node* lRecent; //ptre to the least recent element
    unsigned int m_buckets; //max number of elements in the hash table
    unsigned int m_capacity; //max number of all elements of all linked lists
    unsigned int currCapacity; //number that keeps track of current capacity
};

//constructor
template<typename KeyType, typename ValueType> HashTable<KeyType,ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
{
    bucketList = new Node*[numBuckets];
    mRecent = nullptr;
    lRecent = nullptr;
    m_buckets = numBuckets;
    m_capacity = capacity;
    for(int i = 0; i < numBuckets; i++)
    {
        bucketList[i] = nullptr;
    }
}

//declaration of the compute hash function to be overriden later
template<typename KeyType, typename ValueType>
unsigned int computeHash(KeyType key);


//function that returns the bucket number with a given key
template<typename KeyType, typename ValueType>
unsigned int HashTable<KeyType,ValueType>::getBucketForKey(const KeyType& key) const
{
    // The computeHash function must be defined
    // for each type of key that we use in some
    // hash table.
    unsigned int computeHash(KeyType); // prototype
    unsigned int result = computeHash(key);
    if(result >= m_buckets)
    {
        unsigned int x = result % m_buckets;
        return x;
    }
    return result;
}

//helper function that erases a ptr node
template<typename KeyType, typename ValueType>
void HashTable<KeyType,ValueType>:: erase(Node*& ptr)
{
    //stores the ptr into separte temp variables for changes
    unsigned int getBuck = getBucketForKey(ptr->key);
    Node* temp = ptr;
    Node* temp2 = ptr;
    
    //The next four if statments make the correct changes to the the linked list in order to delete from the non-history linked list
    
    //if the ptr is at the top of the list and there are multiple items in the list
    if(ptr->previous == nullptr && ptr->next != nullptr)
    {
        ptr = ptr->next;
        ptr->previous = nullptr;
        bucketList[getBuck] = ptr;
    }
    
    //if the ptr is in the middle of the list and there are multiple items in the list
    else if(ptr->previous != nullptr && ptr->next != nullptr)
    {
        Node* dest = ptr;
        ptr = ptr->next;
        ptr->previous = dest->previous;
        dest->previous = ptr;
    }
    
    //if the ptr is at the bottom of the list and there are multiple items in the list
    else if(ptr->previous != nullptr && ptr->next == nullptr)
    {
        ptr = ptr->previous;
        ptr->next = nullptr;
    }
    
    //if the ptr is the only element in the list
    else if(ptr->previous == nullptr && ptr->next == nullptr)
    {
        ptr->next = nullptr;
        ptr->previous = nullptr;
        bucketList[getBuck] = nullptr;
    }
    
    
    //The next four if statments make the correct changes to the the linked list in order to delete from the history linked list
    
    //if the ptr is at the top of the list and there are multiple items in the list
    if(temp->hPrevious == nullptr && temp->hNext != nullptr)
    {
        temp = temp->hNext;
        temp->hPrevious = nullptr;
        mRecent = temp;
    }
    
    //if the ptr is in the middle of the list and there are multiple items in the list
    else if(temp->hPrevious != nullptr && temp->hNext != nullptr)
    {
        Node* destt = temp;
        temp = temp->hNext;
        temp->hPrevious = destt->hPrevious;
        destt->hPrevious = temp;
    }

    //if the ptr is at the bottom of the list and there are multiple items in the list
    else if(temp->hPrevious != nullptr && temp->hNext == nullptr)
    {
        temp = temp->hPrevious;
        temp->hNext = nullptr;
        lRecent = temp;
    }
    
    //if the ptr is the only element in the list
    else if(temp->hPrevious == nullptr && temp->hNext == nullptr)
    {
        temp->hNext = nullptr;
        temp->hPrevious = nullptr;
        mRecent = nullptr;
        lRecent = nullptr;
    }
    
    //deletes the node and subtracts from the current capacity
    delete temp2;
    temp2 = nullptr;
    currCapacity--;
}

//helper function called to insert an element to the top of the non-history linked list
template<typename KeyType, typename ValueType>
typename HashTable<KeyType, ValueType>::Node* HashTable<KeyType,ValueType>::insertToFront(Node*& ptr,const KeyType& key, const ValueType& value, bool permanent)
{
    //adds if the linked list is empty
    if (ptr == nullptr)
    {
        Node* p = new Node;
        p->key = key;
        p->val = value;
        p->previous = p->next = nullptr;
        ptr = p;
        return p;
    }
    
    //adds the new value if non existent but the linked list has other elements
    
    //create new node
    Node* p = new Node;
    p->key = key;
    p->val = value;
    
    
    //adds node to the front
    p->next = ptr;
    p->previous = nullptr;
    if(p->next != nullptr)
    {
        Node* x = p->next;
        x->previous = p;
    }
    ptr = p;
    return p;
}

//helper function called to insert an element to the top of the history linked list
template<typename KeyType, typename ValueType>
void HashTable<KeyType,ValueType>:: insertToFrontHist(Node*& ptr,const KeyType& key, const ValueType& value, bool permanent, Node*& passedNode)
{
    //adds if the linked list is empty
    if (ptr == nullptr)
    {
        passedNode->hPrevious = passedNode->hNext = nullptr;
        ptr = passedNode;
        return;
    }
    
    //adds node to the front
    passedNode->hNext = ptr;
    passedNode->hPrevious = nullptr;
    if(passedNode->hNext != nullptr)
    {
        Node* x = passedNode->hNext;
        x->hPrevious = passedNode;
    }
    ptr = passedNode;
}

//destructor
template<typename KeyType, typename ValueType> HashTable<KeyType,ValueType>::~HashTable()
{
    //iterates through the buckets and deletes each ptr
    for(unsigned int i = 0; i < m_buckets; i++)
    {
        Node* p = bucketList[i];
        while(p != nullptr)
        {
            Node* temp = p->next;
            delete p;
            currCapacity--;
            p = temp;
        }
    }
}

//returns a bool according to whether or not the hash table is full
template <typename KeyType, typename ValueType> bool HashTable<KeyType,ValueType>::isFull() const
{
    return currCapacity == m_capacity;
}

//sets or inserts a new element to the hash table
template<typename KeyType, typename ValueType> bool HashTable<KeyType,ValueType>::set(const KeyType& key, const ValueType& value,bool permanent)
{
    int buckElement = getBucketForKey(key);
    Node* p = bucketList[buckElement];
    
    //iterates through the entire list too see if the key already exists
    while(p != nullptr)
    {
        if(p->key == key)
        {
            p->val = value;
            //if the key exists and is not permanent update the specific element by inserting a new element with the same key and changed value and deleting the old element... updates the history accordingly using helper functions
            if(!p->perm)
            {
                KeyType temp = p->key;
                ValueType val = p->val;
                Node* temp2 = insertToFront(bucketList[buckElement], temp, value, p->perm);
                insertToFrontHist(mRecent,temp,val,permanent,temp2);
                currCapacity++;
                erase(p);
            }
            return true;
        }
        p = p->next;
    }
    
    //if the capacity is full return false
    if(isFull())
        return false;
    
    //if key doesn't exist in the hashtable and not full create a new element, put it into the list, and update the history using helper functions
    else
    {
        Node* temp2 = insertToFront(bucketList[buckElement], key, value, permanent);
        //if it's not permanent update the history accordingly
        if(!permanent)
        {
            if(mRecent == nullptr)
            {
                insertToFrontHist(lRecent,bucketList[buckElement]->key,bucketList[buckElement]->val,permanent,temp2);
            }
            insertToFrontHist(mRecent,bucketList[buckElement]->key,bucketList[buckElement]->val,permanent,temp2);
        }
        currCapacity++; //adds 1 to the current capacity
        return true;
    }
    
}

//function that retrieves a specific element
template<typename KeyType, typename ValueType> bool HashTable<KeyType,ValueType>::get(const KeyType& key, ValueType& value) const
{
    //gets the specific key and iterates through the linked list at the that bucket... returns accordingly
    Node* p = bucketList[getBucketForKey(key)];
    while(p != nullptr)
    {
        if(p->key == key)
        {
            value = p->val;
            return true;
        }
        p = p->next;
    }
    return false;
}


//function that touches the element to move it to the most recent element in the history
template<typename KeyType, typename ValueType> bool HashTable<KeyType,ValueType>::touch(const KeyType& key)
{
    //gets the specific key and iterates through the linked list at that bucket
    Node* p = bucketList[getBucketForKey(key)];
    while(p != nullptr)
    {
        //if it is not permanent and finds a match creates the new value to add to the front of the history and deletes the old value... done by using helper functions
        if(p->perm == false && p->key == key)
        {
            KeyType temp = p->key;
            ValueType val = p->val;
            
            Node* n = new Node;
            n->val = p->val;
            n->key = p->key;
            n->previous = p->previous;
            n->next = p->next;
            n->hNext = p->hNext;
            n->hPrevious = p->hPrevious;
            
            insertToFront(bucketList[getBucketForKey(key)], temp, val, p->perm);
            insertToFrontHist(mRecent,temp,val,p->perm,n);
            return true;

        }
        p = p->next;
    }
    return false;
}

//deletes a the oldest element if found using a helper function
template<typename KeyType, typename ValueType> bool HashTable<KeyType,ValueType>::discard(KeyType& key, ValueType& value)
{
    //starts at the least recent ptr and deletes it if it's not permanent using erase and returns true
    Node* p = lRecent;
    while(p != nullptr)
    {
        if(!p->perm)
        {
            key = p->key;
            value = p->val;
            erase(p);
            return true;
        }
        p = p->previous;
    }
    //else return false
    return false;
}




#endif