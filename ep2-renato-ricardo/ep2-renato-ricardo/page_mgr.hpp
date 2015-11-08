#ifndef _PAGE_MGR_HPP
#define _PAGE_MGR_HPP

#include "utils.hpp"

/* -1 se não foi acessado recentemente, >= 0 se foi acessado recentemente, >= INTERRUPT_DT se deve
 * ser resetado.
 */
namespace virt_refs {
  extern double *internal;
  extern int size;
}

inline int p_index(int pi, mem_node *m);

void nru_refresh(double dt);

mem_node *nf_phys_alloc(int size);

mem_node *nru_repl(int req_size);

mem_node *fifo_repl(int req_size);

mem_node *sc_repl(int req_size);

mem_node *lru_repl(int req_size);

#endif /* _PAGE_MGR_HPP */
