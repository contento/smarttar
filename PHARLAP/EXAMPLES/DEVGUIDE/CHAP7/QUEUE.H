/*
 * QUEUE.H - definitions for queue manipulation routines
 */

#define QSIZE 128   /* Length of queue data buffer */

typedef struct
{
    int q_inp;      /* Insert index */
    int q_outp;     /* Extract index */
    char q_buf[QSIZE];  /* Data buffer */
} QUEUE;

queue_init(QUEUE far *);
queue_insert(QUEUE far *, char);
int queue_extract(QUEUE far *);
int queue_status(QUEUE far *);
