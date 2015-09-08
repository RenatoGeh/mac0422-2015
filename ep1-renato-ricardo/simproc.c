/* simproc.c
 * MAC0422 - 2015
 * 
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "utils.h"
#include "simproc.h"

int main(int argc, char *argv[]) {
  /* Thread para gerenciar outros threads. */
  void (*manager) (void);
  
  parse(argv[2]);
  /* Numero de processos disponiveis. */
  n_max_threads = sysconf(_SC_NPROCESSORS_ONLN); 
  n_threads = 0;
  memset(cpu_mask_usage, 0, sizeof(cpu_mask_usage)/sizeof(cpu_mask_usage[0]));

  manager = thread_managers[atoi(argv[1])];
  thread_clock = 0;
  g_clock = clock();

  return 0;
}

void parse(const char *filename) {
  char tmp[100];
  FILE *trace;
  int i;

  trace = fopen(filename, "r");
  
  for (i=0;fgets(tmp, 100, trace) != EOF;++i);

  p_queue = new_queue(i);
  trace_procs = new_queue(i);
  finished_procs = new_queue(i);
  n_procs = i;

  rewind(trace);
  
  double t0, dt, deadline;
  int p;
  char name[M_PROCESS_NAME];
  for (i=0;fscanf("%f %s %f %f %d", &t0, name, &dt, &deadline, &p)!=EOF;++i) {
    process* new_p = new_process(t0, name, dt, deadline, p);
    enqueue(trace_procs, new_p);
  }
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

void process_thread(void *args) {
  process *self;
  clock_t first_clock, i_clock;
  double delta = 0, t_dt;

  first_clock = clock();
  self = (process*) args;
  t_dt = self->dt;

  while (delta < t_dt) {
    i_clock = clock();
    delta += clock()-i_clock;
  }

  self->tf = (clock()-g_clock)/CLOCKS_PER_SEC;
  self->tr = self->tf-self->t0;
  self->status = 0;
  enqueue(finished_procs, self);
}

void fcfs_mgr(void *args) {
  process *p;
  cpu_set_t cpu_mask;  
  int i;

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 > thread_clock)
      tick();

    while (n_threads >= n_max_threads)
      tick();

    pthread_create(&p->id, NULL, &process_thread, (void*) process_thread);
    for (i=0;i<n_max_threads;++i)
      if (!cpu_mask_usage[i]) {
        CPU_ZERO(cpu_mask);
        CPU_SET(i, &cpu_mask);
        cpu_mask_usage[i] = 1;
        pthread_setaffinity_np(p->id, sizeof(cpu_mask), &cpu_mask);
        break;
      }
    p->status = 1;
    ++n_threads;
  }
}

void tick(void) {
  thread_clock = (clock()-g_clock)/CLOCKS_PER_SEC;
}
