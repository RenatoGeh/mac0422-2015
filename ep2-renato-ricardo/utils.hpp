#ifndef _UTILS_HPP
#define _UTILS_HPP

int mem_size;

struct mem_node {
  /*Tipo do bloco: P para processo, L para livre*/
  char t;
  /* Inicio do bloco. */
  int i;
  /* Tamanho do bloco. */
  int s;
  /* Proximo bloco. */
  mem_node *n;
  /* Bloco anterior. */
  mem_node *p;
  /* Constructors. */
  mem_node(char _t, int _i, int _s) : t(_t), i(_i), s(_s) {}
  mem_node(char _t, int _i, int _s, mem_node *_n, mem_node *_p) : t(_t), i(_i), s(_s), n(_n), p(_p) {}
};


int t_size;
int v_size;

int *t_memory;
int *v_memory;

mem_node *t_mem_h;
mem_node *v_mem_h;

#endif /* _UTILS_HPP */
