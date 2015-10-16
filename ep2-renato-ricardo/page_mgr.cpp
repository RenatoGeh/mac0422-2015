#include "page_mgr.hpp"

#include <queue>

#include "utils.hpp"

#define INTERRUPT_DT 2

namespace virt_refs {
  double *internal;
  int size;
}

int p_index(int pi, mem_node *m) {
  return (pi + m->i)/PAGE_SIZE;
}

void nru_refresh(double dt) {
  for (int i=0;i<virt_refs::size;++i) {
    double &r = virt_refs::internal[i];

    if (r > INTERRUPT_DT)
      r = 0;
    else if (r >= 0)
      r += dt;
  }
}

mem_node *nru_repl(int req_size) {
  int f = -1;
  int i, size = 0;

  for (i=0;i<virt_refs::size;++i) {
    if (virt_refs::internal[i] < 0) {
      if (f < 0) f = i;
      size += virt_refs::internal[i];
    } else {
      size = 0;
      f = -1;
    }
  }

  if (size >= req_size) {
    mem_node *it = t_mem_h->n;
    i = 0;
    while (i/PAGE_SIZE < f) {
      i += it->t;
      it = it->n;
    }

    mem_node *start = it;
    mem_node *next = it->n;

    while (size > 0) {
      size -= it->s;
      next = it->n;
      delete it;
      it = next;
      start->s += it->s;
    }

    start->n = it;
    it->p = start;

    return start;
  }

  mem_node *it = t_mem_h->n;
  mem_node *start = it;
  mem_node *next = it->n;

  while (size > 0) {
    size -= it->s;
    next = it->n;
    delete it;
    it = next;
    start->s += it->s;
  }

  start->n = it;
  it->p = start;

  return start;
}

mem_node* nf_phys_alloc(int size) {
  static mem_node *t_last = t_mem_h;
  mem_node *node, *temp;

  for(node = t_last->n; node != t_last; node = node->n) {
    if (node->t == 'L' && node->s >= size){
      if(node->s > size) {
        temp = new mem_node('L', node->i + size, node->s - size, node->n, node);

        node->t = 'P';
        node->s = size;
        node->n = temp;

        t_last = node;
        return (node);
      }
      else{
        node->t = 'P';
        t_last = node;
        return (node);
      }
    }
  }

  return nullptr;
}

static std::queue<mem_node*> page_queue;

mem_node *fifo_repl(int req_size) {
  mem_node *top = page_queue.front();
  mem_node *start = top;

  while (start->s < req_size) {
    start->s += top->s;
    start->n = top->n;
    top->n->p = start;
    delete top;
    page_queue.pop();
    top = page_queue.front();
  }

  page_queue.push(start);

  return start;
}

mem_node *sc_repl(int req_size) { return nullptr; }
mem_node *lru_repl(int req_size) { return nullptr; }
