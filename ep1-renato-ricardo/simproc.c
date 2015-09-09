/* simproc.c
 * MAC0422 - 2015
 * 
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#define _GNU_SOURCE

#include "simproc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

#include "utils.h"

int main(int argc, char *argv[]) {
  /* Thread para gerenciar outros threads. */
  void (*manager) (void);
  
  parse(argv[2]);
  out_file = fopen(argv[3], "w");
  debug_mode = argc==5 && argv[4][0]=='d';

  /* Numero de processos disponiveis. */
  n_max_threads = sysconf(_SC_NPROCESSORS_ONLN); 
  n_threads = 0;
  memset(cpu_mask_usage, 0, sizeof(cpu_mask_usage)/sizeof(cpu_mask_usage[0]));

  sem_unlink("s_mutex");
  sem_init(&s_mutex, 0, 1);

  manager = thread_managers[atoi(argv[1]-1)];
  thread_clock = 0;
  g_clock = clock();

  manager();


  while (p_queue->size > 0) {
    process *fst;
    fst = dequeue(p_queue);
    pthread_join(fst->id, NULL);
  }

  fclose(out_file);

  return 0;
}

void parse(const char *filename) {
  char tmp[100];
  FILE *trace;
  int i, p;
  double t0, dt, deadline;
  char name[M_PROCESS_NAME];

  trace = fopen(filename, "r");
  
  for (i=0;fgets(tmp, 100, trace) != NULL;++i);

  p_queue = new_queue(i);
  trace_procs = new_queue(i);
  finished_procs = new_queue(i);
  n_procs = i;

  rewind(trace);
  
  for (i=0;fscanf(trace, "%lf %s %lf %lf %d", &t0, name, &dt, &deadline, &p)!=EOF;++i) {
    process* new_p = new_proc(t0, name, dt, deadline, p);
    enqueue(trace_procs, new_p);
  }

  fclose(trace);
}

/* Nao precisa disso mais, ja que procs aparecem no trace em ordem nao decrescente.
process *get_ready_proc(void) {
  static start = 0;
  int i;

  for (i=start;i<n_procs;++i)
    if (procs[i].t0 <= thread_clock && procs[i].status < 0) {
      start = i;
      return &procs[i]; 
    }
  
  start = 0;
  return NULL;
}*/

void *process_thread(void *args) {
  process *self;
  clock_t i_clock;
  double delta = 0, t_dt;
  cpu_set_t cpu_mask;
  int i;

  sem_wait(&s_mutex);

  ++n_threads;
  self = (process*) args;
  t_dt = self->dt;
  self->status = 1;
  
  DEBUG("Processo [%s] entrou no sistema.\n", self->name);
  
  for (i=0;i<n_max_threads;++i)
    if (!cpu_mask_usage[i]) {
      CPU_ZERO(&cpu_mask);
      CPU_SET(i, &cpu_mask);
      cpu_mask_usage[i] = 1;
      pthread_setaffinity_np(self->id, sizeof(cpu_mask), &cpu_mask);
      DEBUG("Processo [%s] usando CPU [%d].\n", self->name, i);
      break;
    }

  sem_post(&s_mutex);

  while (delta < t_dt) {
    i_clock = clock();
    delta += ((double)(clock()-i_clock))/((double)CLOCKS_PER_SEC);
  }

  sem_wait(&s_mutex);

  self->tf = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
  self->tr = self->tf-self->t0;
  self->status = 0;

  enqueue(finished_procs, self);
  pthread_getaffinity_np(self->id, sizeof(cpu_mask), &cpu_mask);
  for (i=0;i<M_CPU_CORES;++i)
    if (CPU_ISSET(i, &cpu_mask)) {
      cpu_mask_usage[i] = 0;
      DEBUG("Process [%s] liberando CPU [%d].\n", self->name, i);
    }
  --n_threads;  

  DEBUG("Processo [%s] finalizando e imprimindo linha [\"%s %f %f\"] na saida.\n",
      self->name, self->name, self->tf, self->tr);
  fprintf(out_file, "%s %f %f\n", self->name, self->tf, self->tr);

  sem_post(&s_mutex);

  return NULL;
}

void fcfs_mgr(void) {
  process *p;

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 > thread_clock)
      tick();

    while (n_threads >= n_max_threads)
      tick();
    
    sem_wait(&s_mutex);
    enqueue(p_queue, p);
    sem_post(&s_mutex);

    pthread_create(&p->id, NULL, &process_thread, (void*) p);
  }
}

void sjf_mgr(void) {}
void srtn_mgr(void) {}
void robin_mgr(void) {}
void pschedule_mgr(void) {}
void rdeadline_mgr(void) {}

void tick(void) {
  thread_clock = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
}
