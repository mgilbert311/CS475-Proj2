/* newqueue.c - newqueue */

#include <xinu.h>
#include <stdlib.h>

/**
 * Allocate and initialize a queue
 * @return pointer to queue structure
 */
struct queue *newqueue(void)
{
	//Allocate memory for a new Queue structure (required to be on heap)
	struct queue* q = (struct queue*) malloc(sizeof(struct queue));
	//Initialize the structure
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	//Return pointer to the structure
	return q;
}
