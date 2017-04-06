//-----------------------------------------------------------------------------
// Queue.h
// Header file for Queue ADT
//-----------------------------------------------------------------------------

#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_


// Exported type --------------------------------------------------------------
typedef struct QueueObj* Queue;


// Constructors-Destructors ---------------------------------------------------

// newQueue()
// Returns reference to new empty Queue object.
Queue newQueue(void);

// freeQueue()
// Frees all heap memory associated with Queue *pQ, and sets *pQ to NULL.
void freeQueue(Queue* pQ);


// Access functions -----------------------------------------------------------

// getFront()
// Returns the value at the front of Q.
// Pre: !isEmpty(Q)
unsigned short getFront(Queue Q);

// getLength()
// Returns the length of Q.
int getLength(Queue Q);

// isEmpty()
// Returns true (1) if Q is empty, otherwise returns false (0)
int isEmpty(Queue Q);


// Manipulation procedures ----------------------------------------------------

// Enqueue()
// Places new data element at the end of Q
void Enqueue(Queue Q, unsigned short data);

// Dequeue()
// Deletes element at front of Q
// Pre: !isEmpty(Q)
void Dequeue(Queue Q);


// Other Functions ------------------------------------------------------------

// printQueue()
// Prints data elements in Q on a single line to stdout.
void printQueue(Queue Q);

// equals()
// returns true (1) if A is identical to B, false (0) otherwise
int equals(Queue A, Queue B);

#endif
