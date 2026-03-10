/*
 * QUEUE.C - Queue manipulation routines.
 */

#include <stdlib.h>

#include "queue.h"

#pragma check_stack(off)    /* So we can run at interrupt time */

/* queue_init - Initialize a queue */

queue_init(QUEUE far *qp)
{
    qp->q_inp = 0;
    qp->q_outp = 0;
    return 0;
}

/* queue_insert - Insert a character in a queue.  Return -1 if full */

queue_insert(QUEUE far *qp, char ch)
{
    if(queue_status(qp) >= (QSIZE - 1))
        return -1;
    qp->q_buf[qp->q_inp] = ch;
    if(qp->q_inp < (QSIZE - 1))
        ++qp->q_inp;
    else
        qp->q_inp = 0;
    return 0;
}

/* queue_extract - Extract a character from a queue.  Return -1 if empty */

int queue_extract(QUEUE far *qp)
{
    char ch;

    if(qp->q_outp == qp->q_inp)
        return -1;
    ch = qp->q_buf[qp->q_outp];
    if(qp->q_outp < (QSIZE - 1))
        ++qp->q_outp;
    else
        qp->q_outp = 0;
    return ch;
}

/* queue_status - Return the number of characters in a queue (0 if empty) */

int queue_status(QUEUE far *qp)
{
    if(qp->q_inp >= qp->q_outp)
        return qp->q_inp - qp->q_outp;
    else
        return QSIZE - qp->q_outp + qp->q_inp;
}
