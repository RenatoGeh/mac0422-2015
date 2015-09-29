#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <cstdio>
#include <utility>
#include <functional>

#define MEM_HEADER '\0'

int mem_size;

struct mem_node {
  /*Tipo do bloco: P para processo, L para livre, MEM_HEADER para header*/
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
  ~mem_node() {}
};

int t_size;
int v_size;

int *t_memory;
int *v_memory;

mem_node *t_mem_h;
mem_node *v_mem_h;

FILE *out_phys;
FILE *out_virt;

/* STL std::unordered_set nao aceita ranges. Esse extract faz parte do Boost.
 * Com isso pode-se hashear pairs, tuples, arrays e outros.
*/
template<class T>
inline void hash_combine(std::size_t &seed, const T &v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/* Especializacao do hasher pra pairs. */
struct pair_hash {
  inline std::size_t operator()(const std::pair<int, int> & v) const {
    std::size_t seed = 0;
    
    hash_combine(seed, v.first);
    hash_combine(seed, v.second);
    
    return seed;
  }
};

#define WRITE_PHYS(ptr, size, count) fwrite(out_phys)
#define WRITE_VIRT(ptr, size, count) fwrite(out_virt)

#endif /* _UTILS_HPP */
