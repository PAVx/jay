//-----------------------------------------------------------------------------
// Queue.h
// Header file for Queue ADT
//-----------------------------------------------------------------------------

#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_

#ifndef QUEUE_TXRX_MAXSIZE
#define QUEUE_TXRX_MAXSIZE 128
#endif

// Exported type --------------------------------------------------------------
typedef struct Queue {
   uint8_t length;
   uint16_t buffer[QUEUE_TXRX_MAXSIZE];
   uint8_t head;
   uint8_t tail;
} Queue;

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
uint16_t getFront(Queue Q);

// getLength()
// Returns the length of Q.
int getLength(Queue Q);

// isEmpty()
// Returns true (1) if Q is empty, otherwise returns false (0)
int isEmpty(Queue Q);


// Manipulation procedures ----------------------------------------------------

// Enqueue()
// Places new data element at the end of Q
void Enqueue(Queue Q, uint16_t data);

// Dequeue()
// Deletes element at front of Q
// Pre: !isEmpty(Q)
void Dequeue(Queue Q);

// clear()
// clears the buffer
void clear(Queue Q);
// Other Functions ------------------------------------------------------------

// equals()
// returns true (1) if A is identical to B, false (0) otherwise
int equals(Queue A, Queue B);

#endif
