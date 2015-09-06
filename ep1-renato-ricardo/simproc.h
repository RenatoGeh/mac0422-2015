/* simproc.c
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */ 

#ifndef _SIMPROC_H_
#define _SIMPROC_H_

#include <time.h>

#include "utils.h"

/* Processos no trace. */
process *procs;
/* Numero total de processos no trace. */
int n_procs;
/* Queue de processos. */
queue *p_queue; 
/* Numero maximo de threads rodando ao mesmo tempo. */
int n_max_threads;
/* Numero de threads rodando. */
int n_threads;
/* Clock de inicio do escalonador. */
time_t g_clock;
/* Tempo de execucao do escalonador. */
double thread_clock;

/* Extrai as informacoes do trace. */
void parse(const char *filename);

/* First-come first-served. */
void fcfs_mgr(void *args);

/* Shortest job first. */
void sjf_mgr(void *args);

/* Shortest remaining time next. */
void srtn_mgr(void *args);

/* Round-robin. */
void robin_mgr(void *args);

/* Escalonamento com prioridade. */
void pschedule_mgr(void *args);

/* Escalonamento em tempo real com deadline rigidos. */
void rdeadline_mgr(void *args);

/* A cada chamada retorna um process possivel. Quando impossivel retorna NULL. */
process *get_ready_proc(void);

/* Lista de managers possiveis. */
void (*thread_managers[6]) (void*) = {
  fcfs_mgr, sjf_mgr, srtn_mgr, robin_mgr, pschedule_mgr, rdeadline_mgr,
};

/* Thread dos processos. */
void process_thread(void *args);

#endif /* _SIMPROC_H_ */
