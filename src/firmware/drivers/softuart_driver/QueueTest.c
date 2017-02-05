//-----------------------------------------------------------------------------
// QueueTest.c
// A test client for Queue ADT
//-----------------------------------------------------------------------------

#include<stdio.h>
#include"Queue.h"

int main(int argc, char* argv[]){

   int i;
   Queue A = newQueue();
   Queue B = newQueue();

   for(i=1; i<=10; i++){
      Enqueue(A, i);
      Enqueue(B, 11-i);
   }
   printQueue(A);
   printQueue(B);
   printf("\n");

   for(i=1; i<=6; i++){
      Enqueue(A, getFront(B));
      Dequeue(B);
   }
   printQueue(A);
   printQueue(B);
   printf("\n");

   printf("%s\n", equals(A,B)?"true":"false");
   printf("%s\n", equals(A,A)?"true":"false");

   freeQueue(&A);
   freeQueue(&B);

   return(0);
}
