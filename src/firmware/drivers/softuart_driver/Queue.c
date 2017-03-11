//-----------------------------------------------------------------------------
// Queue.c
// Implementation file for Queue ADT
//-----------------------------------------------------------------------------

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "Queue.h"

// structs --------------------------------------------------------------------
// in h file

// Constructors-Destructors ---------------------------------------------------

// newQueue()
// Returns reference to new empty Queue object.
Queue newQueue() {
   Queue Q;
   Q.length = Q.head = Q.tail = 0; // start at beginning
   return(Q);
}


// freeQueue()
// Frees all heap memory associated with Queue *pQ, and sets *pQ to NULL.S
void freeQueue(Queue* pQ){
   if (pQ != NULL) return;
   return;
   // stack memory doesn't need to be freed
}


// Access functions -----------------------------------------------------------

// getFront()
// Returns the value at the front of Q.
// if isEmpty(Q) returns null character
uint16_t getFront(Queue Q){ // technically should be called getBack()
   if (isEmpty(Q)) return '\0';
   return(Q.buffer[Q.tail]);
}

// getLength()
// Returns the length of Q.
int getLength(Queue Q){
   return(Q.length);
}

// isEmpty()
// Returns true (1) if Q is empty, otherwise returns false (0)
int isEmpty(Queue Q){
   return(Q.length==0);
}

// isFull()
// Returns true (1) if Q is full, otherwise returns false (0)
int isFull(Queue Q) {
  return Q.length==QUEUE_TXRX_MAXSIZE;
}

// Manipulation procedures ----------------------------------------------------

// Enqueue()
// Places new data element at the end of Q if Queue not full
void Enqueue(Queue Q, uint16_t data)
{
   if (isFull(Q)) return;
   Q.buffer[Q.head] = data;
   Q.head = (Q.head + 1) % QUEUE_TXRX_MAXSIZE;
   Q.length++;
}

// Dequeue()
// Deletes element at front of Q
// Pre: !isEmpty(Q)
void Dequeue(Queue Q){
   if( isEmpty(Q) ) return;
   Q.tail = (Q.tail + 1) % QUEUE_TXRX_MAXSIZE;
   Q.length--;
}

void clear(Queue Q) {
   Q.head = Q.tail = Q.length = 0;
}

// Other Functions ------------------------------------------------------------
