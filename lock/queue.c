#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct job {
	struct job *j_next;
	struct job *j_prev;
	pthread_t j_id; /* tells which thread handles this job */
	/* ... more stuff here ... */
};

struct queue {
	struct job *q_head;
	struct job *q_tail;
	pthread_rwlock_t q_lock;
};

/*
 * Initialize a queue.
 */

int queue_init(struct queue *qp) {
	int err;

	qp->q_head = NULL;
	qp->q_tail = NULL;
	err = pthread_rwlock_init(&qp->q_lock, NULL); // 동적 선언
	if (err != 0)
		return(err);

	/* ... continue initialization ... */

	return (0);
}

