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
  
  manager = thread_managers[atoi(argv[1])];
  thread_clock = 0;
  time(&g_clock);

  return 0;
}

void parse(const char *filename) {
  char tmp[100];
  FILE *trace;
  int i;

  trace = fopen(filename, "r");
  
  for (i=0;fgets(tmp, 100, trace) != EOF;++i);

  p_queue = new_queue(i);
  procs = (process*) malloc(i*sizeof(process));
  n_procs = i;

  rewind(trace);
  for (i=0;
        fscanf("%f %s %f %f %d", 
          &procs[i].t0, procs[i].name, procs[i].dt, 
          &procs[i].deadline, &procs[i].p)!=EOF;
        ++i);
}

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
}

/* Incompleto. difftime so da inteiros e portanto sempre vai dar 0. */
void process_thread(void *args) {
  process *self;
  time_t first_clock, i_clock, dt_clock;
  double delta = 0, t_dt;

  time(&first_clock);
  self = (process*) args;
  t_dt = self->dt;

  while (delta < t_dt) {
    time(&i_clock);
    
    time(&dt_clock);
    delta += difftime(i_clock, dt_clock);
  }

  self->tf = difftime(first_clock, dt_clock);
  self->tr = delta;
}

void fcfs_mgr(void *args) {
  time_t c_clock;
  process *p;
  
  p = get_ready_proc();
  while (p != NULL) {
    if (n_threads < n_max_threads) {
      pthread_create(&p->id, NULL, &process_thread, (void*) process_thread);
      p->status = 1;
      ++n_threads;
    }
    p = get_ready_proc();
  }

  thread_clock = difftime(g_clock, c_clock);
}


