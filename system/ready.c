/* ready.c - ready */

#include <xinu.h>

struct queue *readyqueue;	/** ready queue */


/**
 * Put process on ready queue
 * @param pid	ID of process to place on ready queue
 * @param resch	reschedule afterward?
 */
status	ready(pid32 pid, bool8 resch)
{
	register struct procent *prptr;
	//Pass around the int param
	if (isbadpid(pid))
		return(SYSERR);

	// set process state to indicate ready and add to ready list
	prptr = &proctab[pid];
	
	//Set the process' state pointed by prptr to "ready"
	prptr->prstate = PR_READY;
	//Enqueue the process
	enqueue(pid, readyqueue, prptr->prprio);
	

	if (resch == RESCHED_YES){
		resched();
		}
		//Revert back to old priority
	prptr->prprio = prptr->prprioOld;

	return OK;
}
