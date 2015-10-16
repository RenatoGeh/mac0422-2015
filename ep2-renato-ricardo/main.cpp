#include "main.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#include <queue>
#include <unordered_set>
#include <utility>

#include "mem_mgr.hpp"
#include "utils.hpp"

bool interactive;

std::queue<process*> p_queue;

char *args_table[M_ARGS];

int main(int argc, char *argv[]) {
  char *cmd;

  interactive = argc==1;

  cmd = nullptr;
  out_phys = fopen("/tmp/ep2.mem", "wb+");
  out_virt = fopen("/tmp/ep2.vir", "wb+");

  if (!interactive) {
    parse(argv[1]);
    goto cleanup;
  }

  using_history();

  while (true) {
    cmd = readline(PROMPT);

    add_history(cmd);
    extract_args(cmd);

    switch(evaluate_str(args_table[0])) {
      case evaluate_str("carrega"):
        parse(args_table[1]);
        break;
      case evaluate_str("espaco"):
        /* set_mem_mgr(atoi(args_table[1])); */
        break;
      case evaluate_str("substitui"):
        /* set_page_mgr(atoi(args_table[1])); */
        break;
      case evaluate_str("executa"):
        /* run_mem_mgr(atoi(args_table[1])); */
        break;
      case evaluate_str("sai"):
        goto cleanup;
        break;
      default:
        fputs("Erro.\n", stderr);
        break;
    }

    delete cmd;
    for (int i=0;args_table[i]!=NULL;++i) {
      delete[] args_table[i];
      args_table[i] = NULL;
    }
  }

  cleanup:
  fclose(out_phys);
  fclose(out_virt);

  return 0;
}

void parse(char *filename) {
  FILE *trace;
#define M_LINE 300
  char line[M_LINE];

  trace = fopen(filename, "r");

  fscanf(trace, "%d %d", &t_size, &v_size);
  /*Cria as cabeças das listas para a memória virtual e física*/
  t_mem_h = new mem_node(MEM_HEADER, 0, t_size);
  v_mem_h = new mem_node(MEM_HEADER, 0, v_size);

  /*Cria já dois blocos com espaço livre para as listas das memórias virtuais e física*/

  v_mem_h->n = new mem_node('L', 0, v_size, v_mem_h, v_mem_h);
  v_mem_h->p = v_mem_h->n;

  t_mem_h->n = new mem_node('L', 0, t_size, t_mem_h, t_mem_h);
  t_mem_h->p = t_mem_h->n;

  while (1) {
    process *p = new process();

    if (fscanf(trace, "%d %s %d %d", &p->t0, p->name, &p->tf, &p->b) == EOF)
      break;

    fgets(line, M_LINE, trace);

    char *i;
    int pi, ti=pi=-1;

    i = strtok(line, " ");
    while (i != NULL) {
      if (pi >= 0) {
        ti = atoi(i);
        p->pos.push(std::pair<int, int>(pi, ti));
        pi = -1;
      } else
        pi = atoi(i);

      i = strtok(NULL, " ");
    }

    p->id = p_queue.size();
    p->v_alloc_mem = p->t_alloc_mem = nullptr;
    p_queue.push(p);
  }

  /* Considera-se que 255 equivale a -1. */
  write_phys(0, t_size-1, 255);
  write_virt(0, v_size-1, 255);
#undef M_LINE
}

void extract_args(char *line) {
  char *token;
  int i_args = 0;

  token = strtok(line, " ");
  while (token != NULL) {
    int len = strlen(token);
    delete[] args_table[i_args];
    args_table[i_args] = new char[len+1];
    strcpy(args_table[i_args], token);
    token = strtok(NULL, " ");
    ++i_args;
  }
  args_table[i_args] = NULL;
}
