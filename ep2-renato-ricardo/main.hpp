#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <queue>
#include <unordered_set>
#include <utility>

#include "utils.hpp"

#define M_ARGS 10
#define M_NAME 100

#define PROMPT "[ep2]: "

#include <cstring>

/* Se ha um trace file ou e' interativo. */
extern bool interactive;

/* Processo. */
struct process {
  int id;
  /* Tempo inicial do processo. */
  int t0;
  /* Tempo final do processo. */
  int tf;
  /* Nome do processo. */
  char name[M_NAME];
  /* Tamanho em bytes. */
  int b;
  /* Posicoes a serem acessadas. */
  std::queue<std::pair<int, int>> pos;
  /* Memoria virtual alocada. */
  mem_node* v_alloc_mem;
  /* Memoria fisica alocada. */
  mem_node* t_alloc_mem;
};

/* Fila de processos. */
extern std::queue<process*> p_queue;

/* Tabela de argumentos. */
extern char *args_table[M_ARGS];

/* Transforma string em um unsigned integer. */
constexpr unsigned int evaluate_str(const char* str, int h = 0) {
  return !str[h] ? 5381 : (evaluate_str(str, h+1)*33) ^ str[h];
}

/* Extrai comando e argumentos de uma linha. */
void extract_args(char *line);

/* Parse trace file. */
void parse(char *filename);

#endif /* _MAIN_HPP */
