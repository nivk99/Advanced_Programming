/********** I used the code in https://github.com/Pithikos/C-Thread-Pool *************
  @file thpool.h
 AUTHORS: Niv Kotek - 208236315
*/

#ifndef _THPOOL_
#define _THPOOL_
#include "job.h"

typedef struct thpool_* threadpool;

/* ========================== Threadpool ============================ */


/* Initialise thread pool */
threadpool thpool_init(int num_threads);

/* Add work to the thread pool */
int thpool_add_work(threadpool, void (*function_p)(void*), void* arg_p);

/* Wait until all jobs have finished */
void thpool_wait(threadpool);

/* Destroy the threadpool */
void thpool_destroy(threadpool);

/* Pause all threads in threadpool */
void thpool_pause(threadpool);

/* Resume all threads in threadpool */
void thpool_resume(threadpool);


int thpool_num_threads_working(threadpool);




/* Thread */
typedef struct thread
{
	int       id;                        /* friendly id               */
	pthread_t pthread;                   /* pointer to actual thread  */
	struct thpool_* thpool_p;            /* access to thpool          */
} thread,*Pthread;


/* Threadpool */
typedef struct thpool_
{
	Pthread*   threads;                  /* pointer to threads        */
	volatile int num_threads_alive;      /* threads currently alive   */
	volatile int num_threads_working;    /* threads currently working */
	pthread_mutex_t  thcount_lock;       /* used for thread count etc */
	pthread_cond_t  threads_all_idle;    /* signal to thpool_wait     */
	jobqueue  jobqueue;                  /* job queue                 */
} thpool_,*Pthpool_;



/* ========================== THREAD ============================ */


/* Initialize a thread in the thread pool
 *
 * @param thread        address to the pointer of the thread to be created
 * @param id            id to be given to the thread
 * @return 0 on success, -1 otherwise.
 */
int thread_init (Pthpool_ thpool_p, Pthread* thread_p, int id);

/* Sets the calling thread on hold */
void thread_hold(int sig_id);

/* What each thread is doing
*
* In principle this is an endless loop. The only time this loop gets interuppted is once
* thpool_destroy() is invoked or the program exits.
*
* @param  thread        thread that will run this function
* @return nothing
*/
void* thread_do(Pthread thread_p);


/* Frees a thread  */
void thread_destroy (Pthread thread_p);





#endif