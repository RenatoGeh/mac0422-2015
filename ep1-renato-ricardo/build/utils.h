/* utils.h
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */ 

#ifndef _UTILS_H_
#define _UTILS_H_

#include <pthread.h>

/* Processo. */

#define M_PROCESS_NAME 100

typedef struct {
  /* Identificador da thread. */
  pthread_t id;
  /* Tempo de entrada. */
  double t0; 
  /* Nome do processo. */
  char name[M_PROCESS_NAME];
  /* Tempo real de simulacao. */
  double dt;
  /* Tempo de termino. */
  double deadline;
  /* Prioridade. */
  int p;
  /* 1 se esta rodando, 0 se ja foi rodado por completo, -1 se ainda precisa rodar. */
  int status;

  /* Membros aqui sao usados para retorno da thread quando acabado. */
  /* Quando terminou. */
  double tf;
  /* Quanto demorou. */
  double tr;
  
  /* Marca qual CPU esta usando. So para usar quando estiver em real-time scheduling. */
  int using_cpu;
  /* Posicao do processo na queue da CPU. */
  int cpu_queue_pos;
} process;

/* Cria um novo processo. */
process *new_proc(double t0, char *name, double dt, double deadline, int p);

/* Libera o espaco alocado do proc. */
void free_proc(process *proc);

/* Queue. */

typedef struct {
  /* Lista de processos. */
  process **internal;
  /* Tamanho da queue. */
  int size;
  /* Comeco da queue. */
  int _start;
  /* Final da queue. */
  int _end;
  /* Tamanho da capacidade. */
  int capacity;
} queue;

/* Cria uma nova queue. */
queue *new_queue(int size);

/* Libera o espaco alocado da queue. */
void free_queue(queue *q);

/* Enfila processo p em q.
 * IMPORTANTE: considera que nunca vai enfilar numa queue cheia
 */
void enqueue(queue *q, process *p);

/* Tira o primeiro da queue retornando-o. */
process *dequeue(queue *q);

/* Tira todos elementos da queue. */
void clear_queue(queue *q);

/* Retorna iterador pro comeco da fila. */
int queue_start(queue *q);

/* Retorna iterador pro final da fila. */
int queue_end(queue *q);

/* Incrementa o iterador. */
void queue_incr(queue *q, int *it);

/* Priority queue. */

typedef struct {
  process **internal;
  int size;
  int capacity;
  int (*cmp) (process*, process*);
} pqueue;

/* cmp(a, b): -1 se a < b, 0 se a == b, 1 se a > b. */
pqueue *new_pqueue(int size, int (*cmp) (process*, process*));

void free_pq(pqueue *pq);

void enpqueue(pqueue *pq, process *p);

process *depqueue(pqueue *pq);

void clear_pqueue(pqueue *pq);

#endif /* _UTILS_H_ */
