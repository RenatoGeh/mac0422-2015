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
int proc_feitos;

int main(int argc, char *argv[]) {
  /* Thread para gerenciar outros threads. */
  void (*manager) (void);
  
  proc_feitos = 0;

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
  manager = thread_managers[atoi(argv[1])-1];
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
  printf("\n~ %d ~\n", proc_feitos);
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
    DEBUG("Processo [%s] usando CPU [%d].\n", p->name, cpu_i);
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

  /* Comeca secao critica. */
  sem_wait(&s_mutex);

  self = (process*) args;
  t_dt = self->dt;
  self->status = -1;
  
  DEBUG("Processo [%s] entrou no sistema.\n", self->name);
  
  /* Termina seção crítica. */
  sem_post(&s_mutex);

  /* Calcula o tempo real que o processo esta sendo processado */
  while (delta < t_dt) {
    self->status = 1;
    i_clock = clock();
    delta += ((double)(clock()-i_clock))/((double)CLOCKS_PER_SEC);
  }

  /* Comeca secao critica. */
  sem_wait(&s_mutex);

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
  if (self->tf <= self->deadline)
    ++proc_feitos;

  /* Termina secao critica. */
  sem_post(&s_mutex);

  return NULL;
}

void *process_thread_rt(void *args) {
  process *self;
  clock_t i_clock;
  double delta = 0, t_dt;
  int self_using_cpu, self_cpu_queue_pos;

  /* Comeca secao critica. */
  sem_wait(&s_mutex);

  self = (process*) args;
  t_dt = self->dt;
  self->status = -1;
  self_using_cpu = self->using_cpu;
  self_cpu_queue_pos = self->cpu_queue_pos;

  DEBUG("Processo [%s] entrou no sistema.\n", self->name);
  
  /* Termina seção crítica. */
  sem_post(&s_mutex);

  while (delta < t_dt) {
    double real_dt;

    self->status = 1;
    i_clock = clock();
    
    real_dt = ((double)(clock()-i_clock))/((double)CLOCKS_PER_SEC);

    delta += real_dt;

    sem_wait(&s_mutex);
    rt_thread_delta[self_using_cpu] += real_dt;
    printf("%f\n", rt_thread_delta[self_using_cpu]);
    sem_post(&s_mutex);

    pthread_mutex_lock(&rt_lock[self_using_cpu]);
    while (!rt_thread_status[self_using_cpu][self_cpu_queue_pos])
      pthread_cond_wait(&rt_cond[self_using_cpu], &rt_lock[self_using_cpu]);
    pthread_mutex_unlock(&rt_lock[self_using_cpu]);
  }

  /* Comeca secao critica. */
  sem_wait(&s_mutex);

  /* Marca o tempo que o processo acabou e o tempo de duração */
  self->tf = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
  self->tr = self->tf-self->t0;
  self->status = 0;

  enqueue(finished_procs, self);
  detach_thread_affinity(self); 
  cpu_dist[self_using_cpu][self_cpu_queue_pos] = NULL;
  --n_threads;  

  DEBUG("Processo [%s] finalizando e imprimindo linha [\"%s %f %f\"] na saida.\n",
      self->name, self->name, self->tf, self->tr);
  fprintf(out_file, "%s %f %f\n", self->name, self->tf, self->tr);

  /* Termina secao critica. */
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
  int min, i, j;
  int each_cpu;

  each_cpu = n_procs/n_max_threads + 1;
  for (i=0;i<n_max_threads;++i) {
    cpu_dist[i] = (process**) malloc(each_cpu*sizeof(process*));
    for (j=0;j<each_cpu;++j)
      cpu_dist[i][j] = NULL;
    pthread_mutex_init(&rt_lock[i], NULL);
    pthread_cond_init(&rt_cond[i], NULL);
    rt_thread_status[i] = (int*) malloc(each_cpu*sizeof(int));
    for (j=0;j<each_cpu;++j)
      rt_thread_status[i][j] = 0;
    rt_thread_delta[i] = 0;
  }
  
  while (trace_procs->size > 0) {
    p = dequeue(trace_procs);

    while (p->t0 < thread_clock)
      tick();

    sem_wait(&s_mutex);
    
    min = cpu_mask_usage[0];
    j = 0;
    for (i=1;i<n_max_threads;++i)
      if (cpu_mask_usage[i] < min) {
        min = cpu_mask_usage[i];
        j = i;
      }

    for (i=0;i<each_cpu;++i)
      if (cpu_dist[j][i] == NULL) {
        cpu_dist[j][i] = p;
        p->cpu_queue_pos = i;
        break;
      } 
    
    enqueue(p_queue, p);
    
    pthread_create(&p->id, NULL, &process_thread_rt, (void*) p);

    attach_thread_affinity(j, p);
    
    sem_post(&s_mutex);

    for (i=0;i<n_max_threads;++i)
      if (rt_thread_delta[i] > SCHED_QUANTUM) {
        puts("nonono");
        pthread_mutex_lock(&rt_lock[i]);
        for (j=0;j<cpu_mask_usage[i];++j)
          if (cpu_dist[i][j] != NULL) {
            /* Processo j esta rodando na CPU i. */
            if (rt_thread_status[i][j]) {
              int tmp = (i+1)%each_cpu;

              rt_thread_status[i][j] = 0;
              
              for (;tmp!=i;tmp=(tmp+1)%each_cpu)
                if (cpu_dist[i][tmp] != NULL) {
                  rt_thread_status[i][tmp] = 1;
                  break;
                }

              DEBUG("Mudança de contexto [%d]: [%s] -> [%s].\n", 
                  ++context_change, cpu_dist[i][j]->name, cpu_dist[i][tmp]->name);
              break;
            }
          }
        pthread_cond_signal(&rt_cond[i]);
        rt_thread_delta[i] = 0;
        pthread_mutex_unlock(&rt_lock[i]);
      }

  }

  while (1) {
    int k = 0;
    for (i=0;i<n_max_threads;++i)
      k += cpu_mask_usage[i];
    /* Nao tem mais nenhum processo rodando. */
    if (k == 0)
      break;

    for (i=0;i<n_max_threads;++i)
      if (rt_thread_delta[i] > SCHED_QUANTUM) {
        pthread_mutex_lock(&rt_lock[i]);
        for (j=0;j<cpu_mask_usage[i];++j)
          if (cpu_dist[i][j] != NULL) {
            /* Processo j esta rodando na CPU i. */
            if (rt_thread_status[i][j]) {
              int tmp = (i+1)%each_cpu;

              rt_thread_status[i][j] = 0;
              
              for (;tmp!=i;tmp=(tmp+1)%each_cpu)
                if (cpu_dist[i][tmp] != NULL) {
                  rt_thread_status[i][tmp] = 1;
                  break;              
                }

              DEBUG("Mudança de contexto [%d]: [%s] -> [%s].\n", 
                  ++context_change, cpu_dist[i][j]->name, cpu_dist[i][tmp]->name);
              break;
            }
          }
        pthread_cond_signal(&rt_cond[i]);
        rt_thread_delta[i] = 0;
        pthread_mutex_unlock(&rt_lock[i]);
      }

  }

  for (i=0;i<n_max_threads;++i) {
    free(cpu_dist[i]);
    free(rt_thread_status[i]);
  }
}

void pschedule_mgr(void) {}
void rdeadline_mgr(void) {}

void tick(void) {
  thread_clock = ((double)(clock()-g_clock))/((double)CLOCKS_PER_SEC);
}
