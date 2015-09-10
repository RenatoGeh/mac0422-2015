/* simproc.c
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */ 

#ifndef _SIMPROC_H_
#define _SIMPROC_H_

#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#include "utils.h"

#define M_CPU_CORES 16

/* Processos no trace para serem rodados. */
queue *trace_procs;
/* Processos rodados. */
queue *finished_procs;
/* Numero total de processos no trace. */
int n_procs;
/* Queue de processos rodando. */
queue *p_queue; 

/* Priority queue para processos com menor job esperando. */
pqueue *sjf_pqueue;
/* Funcao de comparacao para sjf_pqueue. */
int cmp_sjf_pqueue(process *a, process *b);

/* Priority queue para processos com menor tempo restante. */
pqueue *srtn_pqueue;
/* Funcao de comparacao para srtn_pqueue. */
int cmp_srtn_pqueue(process *a, process *b);

/* Quantum em Round-Robin. */
#define SCHED_QUANTUM 0.050

/* Numero maximo de threads rodando ao mesmo tempo. */
int n_max_threads;
/* Numero de threads rodando. */
int n_threads;
/* Clock de inicio do escalonador. */
clock_t g_clock;
/* Tempo de execucao do escalonador. */
double thread_clock;

/* CPU affinity mask usadas. 1 se sim 0 se nao. */
int cpu_mask_usage[M_CPU_CORES];

/* Semaforo para secoes criticas. */
sem_t s_mutex;

/* Arquivo de saida. */
FILE *out_file;

/* Debug mode. */
char debug_mode;
#define DEBUG(...) if (debug_mode) fprintf(stderr, __VA_ARGS__)

/* Extrai as informacoes do trace. */
void parse(const char *filename);

/* First-come first-served. */
void fcfs_mgr(void);

/* Shortest job first. */
void sjf_mgr(void);

/* Shortest remaining time next. */
void srtn_mgr(void);

/* Round-robin. */
void robin_mgr(void);

/* Escalonamento com prioridade. */
void pschedule_mgr(void);

/* Escalonamento em tempo real com deadline rigidos. */
void rdeadline_mgr(void);

/* A cada chamada retorna um process possivel. Quando impossivel retorna NULL. */
process *get_ready_proc(void);

/* Lista de managers possiveis. */
void (*thread_managers[6]) (void) = {
  fcfs_mgr, sjf_mgr, srtn_mgr, robin_mgr, pschedule_mgr, rdeadline_mgr,
};

/* Thread dos processos. */
void *process_thread(void *args);

/* Does a clock tick on the thread_clock. */
void tick(void);

#endif /* _SIMPROC_H_ */
