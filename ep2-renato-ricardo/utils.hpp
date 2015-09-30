#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <cstdio>
#include <utility>
#include <functional>

#define MEM_HEADER '\0'

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

struct size_node {
  /* Tamanho dos blocos disponiveis. */
  int s;
  
  /*Lista de blocos com tamanho s livre*/
  mem_node *f;
  
  /* Proximo bloco de tamanho dobrado. */
  size_node *n;
  
  /* Bloco anterior de tamanho pela metade. */
  size_node *p;

  /* Constructors. */
  size_node(int _s) : s(_s) {}
  size_node(int _s, mem_node *_f, size_node *_n, size_node *_p) : s(_s), f(_f), n(_n), p(_p) {}
  
  ~size_node() {}
};

/* Tamanho da memória total. */
extern int t_size;
/*Tamanho da memória virtual. */
extern int v_size;

extern mem_node *t_mem_h;
extern mem_node *v_mem_h;

extern FILE *out_phys;
extern FILE *out_virt;

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

/* Escreve o valor val em byte no range [0, f]. */
void write_phys(int i, int f, char val);
void write_virt(int i, int f, char val);

/* Listas crescentes de listas de tamanhos de espaços livres (multiplos de 2), 
 * dobrando a cada node e voltando até a cabeca.
 */
extern size_node *t_size_h;
extern size_node *v_size_h;

/* Todos limites são log2 de seu valor real: se o limite superior virtual 
 * for 1024, v_ls será 10 por exemplo 
 */

/* Limite superior para a memória total */
extern int t_ls;
/* Limite superior para a memória virtual */
extern int v_ls;
/* Limite inferior usado em ambas memórias */
extern int li;

/* Atualiza os valores dos limites superiores para a memória total e virtual.
 * Importante: precisa que já tenha recebido os valores t_size e v_size
 */
void get_limits(void);

/* Cria a "lista de listas" para o quick fit, se baseando nos limites superiores e inferiores */
void create_qf(void);

/* Próximas duas funções distribuem um pedaço da memória total ou virtual (que começa da posição pos
 * e tem tamanho size) da seguinte forma:
 *  -Divide em 4 esse espaço, e aproxima até o maior multiplo de 2 menor que esse valor.
 *  -Se o resultado for maior que o limite inferior, separa 3/4 de size como espaço livre nas listas e chama a função
 *  recursivamente para o restante.
 *  -Caso o resultado seja igual ou menor que o limite inferior, separa o maior numero de intervalos com tamanho limite
 *  inferior como espaço livre na lista do quick fix
 */
void t_dis_mem(int pos, int size);
void v_dis_mem(int pos, int size);

/* Próximas duas funções separam os blcoos de memória total ou virtual de amanho size
 * (sempre multiplo de 2) como livre nas listas do quick fit
 */
void t_separate(int pos, int size);
void v_separate(int pos, int size);

#endif /* _UTILS_HPP */
