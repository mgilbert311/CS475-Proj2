/* queue.c - enqueue, dequeue, isempty, nonempty, et al. */

#include <xinu.h>
#include <stdlib.h>

/**
 * Prints out contents of a queue
 * @param q	pointer to a queue
 */
void	printqueue(struct queue *q)
{
	kprintf("[ ");
	struct qentry *elem = q->head;
	while(elem != NULL){
		kprintf("(pid=%d)", elem->pid);
		elem = elem->next;

		//Only print a comma if there are more to follow
		if(elem != NULL){
			kprintf(", ");
		}
	}
	kprintf("]");
}

/**
 * Checks whether queue is empty
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	isempty(struct queue *q)
{
	return (q->size == 0);
}

/**
 * Checks whether queue is nonempty
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	nonempty(struct queue *q)
{
	return (q == NULL || q->size >0);
}


/**
 * Checks whether queue is full
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	isfull(struct queue *q)
{
	//Check if there are >= least NPROC processes in the queue
	return (q->size >= NPROC);

}


/**
 * Insert a process at the tail of a queue
 * @param pid	ID process to insert
 * @param q	Pointer to the queue to use
 *
 * @return pid on success, SYSERR otherwise
 */
pid32 enqueue(pid32 pid, struct queue *q, int32 key)
{
       //Taken David Chiu's Enqueue method
       
        //If queue is full or pid is illegal, return SYSERR
		if(isfull(q)){
			return SYSERR;
		}
        //Allocate space on heap for a new QEntry
        struct qentry *procToEnqueue = (struct qentry*) malloc(sizeof(struct qentry));
        //Initialize entry
        procToEnqueue->pid = pid;
        procToEnqueue->key = key;
        procToEnqueue->next = NULL;
        procToEnqueue->prev = NULL;


		struct qentry *current;
		struct qentry *previous;
		current = q->head;

        //Determine where the process should be placed based on priority
		while(current != NULL && current->key >= key){
			current = current->next;
		}		
				//Put at the tail
				if(current == NULL){
					previous = q->tail;
				}else{
					previous = current->prev;
					
				}
				
				
				procToEnqueue->prev = previous;
				procToEnqueue->next = current;
				
				if(previous != NULL){
					previous->next = procToEnqueue;
				}
				if(current != NULL){
					current->prev = procToEnqueue;
				}
				if(previous == NULL){
					q->head = procToEnqueue;
				}
				if(current == NULL){
					q->tail = procToEnqueue;
				}
				
				
        
        q->size++;
        //Return the pid on success
        return OK;
}


/**
 * Remove and return the first process on a list
 * @param q	Pointer to the queue to use
 * @return pid of the process removed, or EMPTY if queue is empty
 */
pid32 dequeue(struct queue *q)
{
        //Return EMPTY if queue is empty
		if(isempty(q)){
			return EMPTY;
		}
		
        	//Get the head entry of the queue
			struct qentry *head = q->head;
			struct qentry *newHead = head->next;
			pid32 pid = head->pid; 

        	//Unlink the head entry from the rest

			if(newHead != NULL){
				newHead->prev = NULL;
			}else{
				q->tail = NULL;
			}
        	//Free up the space on the heap
			q->head = newHead;
			q->size--;
			free(head, sizeof(struct qentry));

			//Decrement q size

        	//Return the pid on success
			return pid;
		
}


/**
 * Finds a queue entry given pid
 * @param pid	a process ID
 * @param q	a pointer to a queue
 * @return pointer to the entry if found, NULL otherwise
 */
struct qentry *getbypid(pid32 pid, struct queue *q)
{
	//Return NULL if queue is empty or if an illegal pid is given
	if(isempty(q) || isbadpid(pid)){
		return NULL;
	}
	else{
		//Find the qentry with the given pid
		struct qentry* entry = q->head;
		while(entry != NULL){
			if(entry->pid == pid){
				return entry;
			}
			else{
				entry = entry->next;
			}
		}
		return NULL;
	}
}

/**
 * Remove a process from the front of a queue (pid assumed valid with no check)
 * @param q	pointer to a queue
 * @return pid on success, EMPTY if queue is empty
 */
pid32	getfirst(struct queue *q)
{
	//Return EMPTY if queue is empty
	if(isempty(q)){
		return EMPTY;
	}
	else{
		//Remove process from head of queue and return its pid
		return dequeue(q);

	}
}

/**
 * Remove a process from the end of a queue (pid assumed valid with no check)
 * @param q	Pointer to the queue
 * @return pid on success, EMPTY otherwise
 */
pid32	getlast(struct queue *q)
{
	//Return EMPTY if queue is empty
	if(isempty(q)){
		return EMPTY;
	}
	else{//Remove process from tail of queue and return its pid
		//Get tail
		struct qentry* tail = q->tail;
		//Get its pid
		pid32 pid = tail->pid;
		//free tail
		free(tail, sizeof(tail));
		//Decrement q size
		q->size--;
		//return pid
		return pid;
	}
}



/**
 * Remove a process from an arbitrary point in a queue
 * @param pid	ID of process to remove
 * @param q	Pointer to the queue
 * @return pid on success, SYSERR if pid is not found
 */
pid32	remove(pid32 pid, struct queue *q)
{
	//Return EMPTY if queue is empty
	if(isempty(q)){
		return NULL;
	}

		 //Remove process identified by pid parameter from the queue and return its pid
		//Get entry
		struct qentry* entryToRemove = getbypid(pid, q);
		if(entryToRemove != NULL){ //pid exists
			//Update pointers
			if(q->head == entryToRemove){
				return getfirst(q);
			}
			else if(q->tail == entryToRemove){
				return getlast(q);
			}
			else{
				//Adjust neighbor pointers
				entryToRemove->prev->next = entryToRemove->next;
				entryToRemove->next->prev = entryToRemove->prev;
				//Store pid
				pid32 pid = entryToRemove->pid;
				//Free mem
				free(entryToRemove, sizeof(entryToRemove));
				//Decrment q size
				q->size--;
				//Return pid
				return pid;
			}
		}
		else{ //pid can't be found
			return SYSERR;
		}
	}

