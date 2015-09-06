/* utils.c
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#include "utils.h"

/* Processo. */

process *new_proc(int t0, char *name, int dt, int deadline, int p) {
  process *inst;

  inst = (process*) malloc(sizeof(process));
  inst->t0 = t0;
  inst->name = (char*) malloc((strlen(name)+1)*sizeof(char));
  strcpy(inst->name, name);
  inst->dt = dt;
  inst->deadline = deadline;
  inst->p = p;
  inst->status = -1;

  inst->tr = inst->tf = -1;

  return inst;
}

void free_proc(process *proc) {
  free(proc->name);
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
  int i, cap = q->capacity;

  for (i=0;i<cap;++i)
    free(inst->internal[i]);
  free(inst->internal);
  
  free(q);
}

void enqueue(queue *q, process *p) {
  q->internal[q->_end] = p;
  q->_end = (q->end + 1) % q->capacity;
  ++(q->size);
}

process *dequeue(queue *q) {
  process *res;

  if (q->size >= q->capacity)
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

