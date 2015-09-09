/* utils.c
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#include "utils.h"

#include <stdlib.h>
#include <string.h>

/* Processo. */

process *new_proc(double t0, char *name, double dt, double deadline, int p) {
  process *inst;

  inst = (process*) malloc(sizeof(process));
  inst->t0 = t0;
  strcpy(inst->name, name);
  inst->dt = dt;
  inst->deadline = deadline;
  inst->p = p;
  inst->status = -1;

  inst->tr = inst->tf = -1;

  return inst;
}

void free_proc(process *proc) {
  free(proc);
}

/* Queue. */

queue *new_queue(int size) {
  queue *inst;
  int i;

  inst = (queue*) malloc(sizeof(queue));
  inst->internal = (process**) malloc(size*sizeof(process*));
  for (i=0;i<size;++i)
    inst->internal[i] = NULL;
  inst->_start = inst->_end = 0;
  inst->capacity = size;
  inst->size = 0;

  return inst;
}

void free_queue(queue *q) {
  free(q->internal); 
  free(q);
}

void enqueue(queue *q, process *p) {
  q->internal[q->_end] = p;
  q->_end = (q->_end + 1) % q->capacity;
  ++(q->size);
}

process *dequeue(queue *q) {
  process *res;

  if (q->size > q->capacity || q->size <= 0)
    return NULL;

  res = q->internal[q->_start];
  q->internal[q->_start] = NULL;
  q->_start = (q->_start + 1) % q->capacity;
  --(q->size);

  return res;
}

void clear_queue(queue *q) {
  process *cursor;
  int i, cap = q->capacity, e = q->_end;

  if (q->size == 0)
    return;

  i = q->_start;
  do {
    cursor = q->internal[i];
    free(cursor);
    q->internal[i] = NULL;
    i = (i+1) % cap;
  } while (i != e);

  q->_start = q->_end = 0;
  q->size = 0;
}

/* Priority queue. */

pqueue *new_pqueue(int size, int (*cmp) (process*, process*)) {
  pqueue *inst;

  inst = (pqueue*) malloc(sizeof(pqueue));
  inst->internal = (process**) malloc((size+1)*sizeof(process*));
  inst->size = 0;
  inst->capacity = size+1;
  inst->cmp = cmp;

  return inst;
}

void free_pq(pqueue *pq) {
  free(pq->internal);
  free(pq);
}

void _insert_pq(process *np, process **heap, int size, int (*cmp) (process*, process*)) {
  int i;
  process *tmp;

  i = size + 1;
  heap[i] = np;
  
  while (cmp(heap[i], heap[i/2]) < 0 && i > 1) {
    tmp = heap[i];
    heap[i] = heap[i/2];
    heap[i/2] = tmp;

    i /= 2;
  }
}

void _sdown_pq(process **heap, int size, int i, int (*cmp) (process*, process*)) {
  int child;
  process *tmp;

  while (1) {
    child = i*2;
    
    if (child > size)
      break;
    else if ((child < size) && (cmp(heap[child], heap[child+1]) > 0))
      ++child;

    if (cmp(heap[child], heap[i]) < 0) {
      tmp = heap[child];
      heap[child] = heap[i];
      heap[i] = tmp;
      i = child;
    } else break;
  }
}

process* _rm_min_pq(process **heap, int size, int (*cmp) (process*, process*)) {
  process *top = heap[1];

  heap[1] = heap[size];
  --size;
  _sdown_pq(heap, size, 1, cmp);

  return top;
}

void enpqueue(pqueue *pq, process *p) {
  _insert_pq(p, pq->internal, pq->size, pq->cmp);
  ++(pq->size);
}

process *depqueue(pqueue *pq) {
  process *top;
  
  top = _rm_min_pq(pq->internal, pq->size, pq->cmp);
  --(pq->size);
  
  return top;
}

void clear_pqueue(pqueue *pq) {
  int i, size;

  size = pq->size;
  
  for (i=0;i<size;++i) {
    free_proc(pq->internal[i]);
    pq->internal[i] = NULL;
  }

  pq->size = 0;
}
