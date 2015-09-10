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

  /* Seta todas as cpus como disponiveis. */
  memset(cpu_mask_usage, 0, sizeof(cpu_mask_usage)/sizeof(cpu_mask_usage[0]));

  /* Inicializa o semáforo para todas as threads */
  sem_unlink("s_mutex");
  sem_init(&s_mutex, 0, 1);

  /* Pela entrada defini qual escalonador vai usar */
  manager = thread_managers[atoi(argv[1]-1)];
  thread_clock = 0;
  g_clock = clock();

  /* Inicializa o escalonador */
  manager();

  /* Loop que checa se tem algum processo não terminado */
  while (p_queue->size > 0) {
    process *fst;
    fst = dequeue(p_queue);
    pthread_join(fst->id, NULL);
    free_proc(fst);
  }

  free_queue(p_queue);
  free_queue(trace_procs);
  free_queue(finished_procs);

  fclose(out_file);

  sem_destroy(&s_mutex);

  return 0;
}

void parse(const char *filename) {
  char tmp[100];
  FILE *trace;
  int i, p;
  double t0, dt, deadline;
  char name[M_PROCESS_NAME];

  trace = fopen(filename, "r");
  

  /* Le o arquivo de entrada e conta o numero de processos */
  for (i=0;fgets(tmp, 100, trace) != NULL;++i);

  p_queue = new_queue(i);
  trace_procs = new_queue(i);
  finished_procs = new_queue(i);
  n_procs = i;

  rewind(trace);
  
  /* Enfila os processos */
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

/* Se -1, entao vai tentar achar por si proprio qualquer um que esteja sem nada pra fazer.
 * Senao entra na CPU dada.
 */
int attach_thread_affinity(int cpu_i, process *p) {
  cpu_set_t cpu_mask;
  int i;

  CPU_ZERO(&cpu_mask);

  if (cpu_i < 0) {
    for (i=0;i<n_max_threads;++i)
      if (!cpu_mask_usage[i]) {
        CPU_SET(i, &cpu_mask);
        ++cpu_mask_usage[i];
        pthread_setaffinity_np(p->id, sizeof(cpu_mask), &cpu_mask);
        DEBUG("Processo [%s] usando CPU [%d].\n", p->name, i);
        return i;
      }
  } else {
    CPU_SET(cpu_i, &cpu_mask);
    ++cpu_mask_usage[cpu_i];
    pthread_setaffinity_np(p->id, sizeof(cpu_mask), &cpu_mask);
    DEBUG("Processo [%s] usando CPU [%d].\n", p->name, i);
  }

  return cpu_i;
} 

int detach_thread_affinity(process *p) {
  cpu_set_t cpu_mask;
  int i, k=-1;

  pthread_getaffinity_np(p->id, sizeof(cpu_mask), &cpu_mask);
  for (i=0;i<M_CPU_CORES;++i)
    if (CPU_ISSET(i, &cpu_mask)) {
      --cpu_mask_usage[i];
      DEBUG("Processo [%s] liberando CPU [%d].\n", p->name, i);
      k = i;
    }

  return k;
}

void *process_thread(void *args) {
  process *self;
  clock_t i_clock;
  double delta = 0, t_dt;

  sem_wait(&s_mutex);

  /* Seção crítica*/

  self = (process*) args;
  t_dt = self->dt;
  self->status = -1;
  
  DEBUG("Processo [%s] entrou no sistema.\n", self->name);
  
  sem_post(&s_mutex);

  /* Calcula o tempo real que o processo esta sendo processado */
  while (delta < t_dt) {
    self->status = 1;
    i_clock = clock();
    delta += ((double)(clock()-i_clock))/((double)CLOCKS_PER_SEC);
  }

  sem_wait(&s_mutex);

  /* Seção crítica */

  /* Marca o tempo que o processo acabou e o tempo de duração */
  self->tf = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
  self->tr = self->tf-self->t0;
  self->status = 0;

  enqueue(finished_procs, self);
  detach_thread_affinity(self); 
  --n_threads;  

  DEBUG("Processo [%s] finalizando e imprimindo linha [\"%s %f %f\"] na saida.\n",
      self->name, self->name, self->tf, self->tr);
  fprintf(out_file, "%s %f %f\n", self->name, self->tf, self->tr);

  sem_post(&s_mutex);

  return NULL;
}

/* First-Come First-Served */
void fcfs_mgr(void) {
  process *p;

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 > thread_clock)
      tick();

    while (n_threads >= n_max_threads)
      tick();
   
    sem_wait(&s_mutex);

    /* Seção crítica */
    enqueue(p_queue, p);
    ++n_threads;
    pthread_create(&p->id, NULL, &process_thread, (void*) p);
    attach_thread_affinity(-1, p);
    sem_post(&s_mutex); 
  }
}

int cmp_sjf_pqueue(process *a, process *b) {
  return (a->dt < b->dt)? -1 : (a->dt == b->dt)? 0 : 1;
}

/* Shortest Job First*/
void sjf_mgr(void) {
  process *p, *top;

  sjf_pqueue = new_pqueue(n_procs, &cmp_sjf_pqueue);

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 > thread_clock)
      tick();
 
    sem_wait(&s_mutex);

    /* Seção crítica */
    enpqueue(sjf_pqueue, p);
    /* Os primeiros processos já são começados enquanto possui CPUs disponíveis */
    while (n_threads < n_max_threads && sjf_pqueue->size > 0) {
      top = depqueue(sjf_pqueue);
      enqueue(p_queue, top);
      ++n_threads;
      pthread_create(&top->id, NULL, &process_thread, (void*) top);
      attach_thread_affinity(-1, top);
    }    
    
    sem_post(&s_mutex);
  }

  while (sjf_pqueue->size > 0) {
    while (n_threads >= n_max_threads)
      tick();

    sem_wait(&s_mutex);
    /* Seção crítica */

    /* Se ainda resta processos não completados, os completa por prioridade (mais rápido) */
    while (n_threads < n_max_threads && sjf_pqueue->size > 0) {
      top = depqueue(sjf_pqueue);
      enqueue(p_queue, top);
      ++n_threads;
      pthread_create(&top->id, NULL, &process_thread, (void*) top);
      attach_thread_affinity(-1, top);
    }    
    
    sem_post(&s_mutex);
  }

  free_pq(sjf_pqueue);
}

int cmp_srtn_pqueue(process *a, process *b) {
  int a_val, b_val;

  a_val = a->deadline-thread_clock;
  b_val = b->deadline-thread_clock;

  return (a_val < b_val)? -1 : (a_val == b_val)? 0 : 1;
}

/* Shortest Remaining Time Next */
void srtn_mgr(void) {
  process *p, *top;

  srtn_pqueue = new_pqueue(n_procs, &cmp_srtn_pqueue);

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 > thread_clock)
      tick();
 
    sem_wait(&s_mutex);
    /* Seção crítica */

    enpqueue(srtn_pqueue, p);
    /* Os primeiros processos já são começados enquanto possui CPUs disponíveis */
    while (n_threads < n_max_threads && srtn_pqueue->size > 0) {
      top = depqueue(srtn_pqueue);
      enqueue(p_queue, top);
      ++n_threads;
      pthread_create(&top->id, NULL, &process_thread, (void*) top);
      attach_thread_affinity(-1, top);
    }    
    
    sem_post(&s_mutex);
  }

  while (srtn_pqueue->size > 0) {
    while (n_threads >= n_max_threads)
      tick();

    sem_wait(&s_mutex);
    /* Seção crítica */

    /* Se ainda resta processos não completados, os completa por prioridade (menor tempo restante) */
    while (n_threads < n_max_threads && srtn_pqueue->size > 0) {
      top = depqueue(srtn_pqueue);
      enqueue(p_queue, top);
      ++n_threads;
      pthread_create(&top->id, NULL, &process_thread, (void*) top);
      attach_thread_affinity(-1, top);
    }    
    
    sem_post(&s_mutex);
  }

  free_pq(srtn_pqueue);
}

void robin_mgr(void) {
  process *p;
  int min, i;

  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 < thread_clock)
      tick();

    min = cpu_mask_usage[0];
    for (i=1;i<n_max_threads;++i)
      if (cpu_mask_usage[i] < min)
        min = cpu_mask_usage[i];

       
  }
}

void pschedule_mgr(void) {}
void rdeadline_mgr(void) {}

void tick(void) {
  thread_clock = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
}
