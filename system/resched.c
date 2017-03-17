/* resched.c - resched */

#include <xinu.h>

/**
 * Reschedule processor to next ready process
 *
 */
void	resched(void)		// assumes interrupts are disabled
{
	struct procent *ptold;	// ptr to table entry for old process
	struct procent *ptnew;	// ptr to table entry for new process




	// If rescheduling is deferred, record attempt and return
	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}
	
	
	// Point to process table entry for the current (old) process
	ptold = &proctab[currpid];


	//Check ptold's state. If it's running, put it on the ready queue and change state to ready
	if(ptold->prstate == PR_CURR){
		//ready(currpid, RESCHED_NO, key);
		ptold->prstate = PR_READY;
		enqueue(currpid, readyqueue, ptold->prprio);
		//After it is scheduled, revert back the prprio to original

	}
	//Dequeue next process off the ready queue and point ptnew to it
	currpid = dequeue(readyqueue);
	ptnew = &proctab[currpid];
	//Change its state to "current" (i.e., running)
	ptnew->prstate = PR_CURR;

	int i;
	
		

	//If the aging policy is turned on run this way

	if(AGING == TRUE){
		//Loop through the readyqueue and update the process' priority
	struct qentry *qTemp = readyqueue->head;
		
		//while(qTemp != NULL){
		for(i=0;i <readyqueue->size; i++){
		
			struct procent *temp = &proctab[qTemp->pid];
			//Don't update these processes' priority
			if(temp == NULLPROC || temp == ptold || temp == ptnew ){
				continue;
			}else{
				qTemp->key++;
			}
			qTemp = qTemp->next;
		}
		
	}
	
	// Context switch to next ready process
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	// Old process returns here when resumed
	return;
}
