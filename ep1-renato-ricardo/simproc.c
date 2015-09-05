/* simproc.c
 * MAC0422 - 2015
 * 
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#include <stdio.h>

#include "utils.h"

process *procs;
queue *p_queue; 
int n_threads;

/* Extrai as informacoes do trace. */
void parse(const char *filename);

int main(int argc, char *argv[]) {
  parse(argv[1]);
  /* Numero de processos disponiveis. */
  n_threads = sysconf(_SC_NPROCESSORS_ONLN); 
  
  

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

  rewind(trace);
  for (i=0;
        fscanf("%f %s %f %f %d", 
          &procs[i].t0, procs[i].name, procs[i].dt, 
          &procs[i].deadline, &procs[i].p)!=EOF;
        ++i);
}
