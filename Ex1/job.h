/********* I used the code in https://github.com/Pithikos/C-Thread-Pool *************
  @file job.h
  AUTHORS: Niv Kotek - 208236315
*/

#ifndef _JOB_
#define _JOB_

#include "binary_semaphore.h"


/* Job */
typedef struct job
{
	struct job*  prev;                   /* pointer to previous job   */
	void   (*function)(void* arg);       /* function pointer          */
	void*  arg;                          /* function's argument       */
} job,*Pjob;


/* Job queue */
typedef struct jobqueue
{
	pthread_mutex_t rwmutex;             /* used for queue r/w access */
	Pjob  front;                         /* pointer to front of queue */
	Pjob  rear;                          /* pointer to rear  of queue */
	Pbsem has_jobs;                      /* flag as binary semaphore  */
	int   len;                           /* number of jobs in queue   */
} jobqueue,*Pjobqueue;


/* Initialize queue */
int   jobqueue_init(Pjobqueue jobqueue_p);

/* Clear the queue */
void  jobqueue_clear(Pjobqueue jobqueue_p);

/* Add (allocated) job to queue*/
void  jobqueue_push(Pjobqueue jobqueue_p, Pjob newjob_p);

/* Get first job from queue(removes it from queue)
 * Notice: Caller MUST hold a mutex
 */
Pjob jobqueue_pull(Pjobqueue jobqueue_p);

/* Free all queue resources back to the system */
void  jobqueue_destroy(Pjobqueue jobqueue_p);

#endif