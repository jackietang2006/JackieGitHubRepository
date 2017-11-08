#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<string>
#include <mutex>
#include <utility>
#include <tuple>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <netinet/in.h>

#define NFLOWMSGSIZE 2048
#define NFLOWBUFSIZE 10000

#define SHAREDMEMORY_SIZE 2064
//sharedMemory size = (2048)NFLOWMSGSIZE
//                  + (4)sizeof MsgLength
//                  + (4)sizeof clientaddr.sin_addr.s_addr
//                  + (2)sizeof BuffPool
//                  + (2)sizeof MsgQueue
//                  + (4)number of MsgsDecoded

/*
    96 bit (12 bytes) pseudo header needed for udp header checksum calculation
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

/*
 * error - wrapper for perror
 */
void error(const char *msg);

/*
    Generic checksum calculation function
    Calculating checksum in network mode
*/
unsigned short csum(unsigned short *ptr,int nbytes);

#include <iostream>
#include <stdexcept>

using namespace std;
 
template <class T>
class FIFO
{
public:
 
    struct Node
    {
        T value;
        Node *next;
 
        Node() : next (NULL) {}
 
        void set(T newValue, Node *newNext)
        { 
            value = newValue;
            next = newNext;
        }
 
        void setNext(Node *newNext)
        {
            next = newNext;
        }
    };
 
    Node *head;
    Node *tail;
    short mSize;
 
    FIFO() : head (NULL), tail (NULL), mSize(0) {}
 
    void enqueue(T _value)
    {
      mSize ++;
      Node *newNode = new Node;
 
      newNode->set(_value, NULL);
      if (head == NULL)
      {
        head = newNode;
      }
      else
      {
        tail->setNext(newNode);
      }
      tail = newNode;
    }
 
    Node* dequeue()
    {
      if (head == NULL)
      {
        return NULL;
      }

      mSize --;
      Node *result = head;
      head = head->next;
      return result;
    }

    short& size()
    {
      return mSize;
    }
};

class SharedMemory
{
public:
    enum ShmType {SERVER, CLIENT};

    SharedMemory(key_t key, int size, ShmType t);
 
    void putShm(const void* cpyShmFrom);
    const char* getShm()
    {
      return mShm;
    }

private:
    key_t mKey;
    int mShmSize;
    ShmType mType;

    int mShmID;
    const char* mShm;
};

#endif
