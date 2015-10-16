#include "mem_mgr.hpp"

#include <cmath>
#include <cstdio>

#include "main.hpp"
#include "utils.hpp"
#include "page_mgr.hpp"

enum MEM_ALG {
  FF = 1,
  NF,
  QF,
};

enum PAGE_ALG {
  NRU = 1,
  FIFO,
  SC,
  LRU,
};

mem_node* (*mem_alg_mgrs[3]) (int) = {ff_aloc, nf_aloc, qf_aloc};
mem_node* (*page_alg_mgrs[4]) (int) = {nru_repl, fifo_repl, sc_repl, lru_repl};

int mem_alg = MEM_ALG::FF;
int page_alg = PAGE_ALG::NRU;

int mem_status_dt = -1;

void set_mem_mgr(int opt) {
  opt = mem_alg;
  if (mem_alg == MEM_ALG::QF)
    create_qf();
}

void set_page_mgr(int opt) {
  opt = page_alg;
}

void run_mem_mgr(int dt) {
  mem_status_dt = dt;
  mem_node* (*mgr) (int);
  mem_node* (*page_mgr) (int);
  double d_t = dt;
  int count = 0;

  mgr = mem_alg_mgrs[mem_alg-1];
  page_mgr = page_alg_mgrs[page_alg-1];

  if (page_alg == PAGE_ALG::NRU || page_alg == PAGE_ALG::SC) {
    virt_refs::internal = new double[(virt_refs::size = v_size/PAGE_SIZE)];
    for (int i=0;i<virt_refs::size;++i)
      virt_refs::internal[i] = -1;
  }

  double t_secs = 0;
  clock_t i_clock = clock();
  while (!p_queue.empty()) {
    process *top = p_queue.front();
    std::pair<int, int> n_pos = top->pos.front();
    top->pos.pop();
    double rt = top->tf - top->t0;

    mem_node *alloc = mgr(top->b);
    alloc->t = 'P';
    top->v_alloc_mem = alloc;
    write_virt(top->v_alloc_mem->i, top->v_alloc_mem->i+top->v_alloc_mem->s, top->id);

    while (top->t0 > t_secs)
      t_secs = ((double)(clock()-i_clock)/((double)CLOCKS_PER_SEC));

    while (rt > 0) {
      clock_t r_clock = clock();

      if (n_pos.second <= t_secs) {
        if (top->t_alloc_mem != nullptr) {
          if (page_alg == PAGE_ALG::NRU || page_alg == PAGE_ALG::SC)
            virt_refs::internal[n_pos.first + top->t_alloc_mem->i] = 0;
        } else /* PAGE FAULT */ {
          int _page_size = 16, k = top->v_alloc_mem->s;
          for (;_page_size<k;_page_size<<=1);
          top->t_alloc_mem = nf_phys_alloc(_page_size);
          if (top->t_alloc_mem == nullptr)
            top->t_alloc_mem = page_mgr(_page_size);
          top->t_alloc_mem->t = 'P';
          int st = top->t_alloc_mem->i;
          write_phys(st, st + top->t_alloc_mem->s, top->id);
        }
      }

      if (page_alg == PAGE_ALG::NRU || page_alg == PAGE_ALG::SC)
        nru_refresh((double)(clock()-r_clock)/((double)CLOCKS_PER_SEC));

      double cycle_secs = ((double)(clock()-r_clock)/((double)CLOCKS_PER_SEC));

      if (d_t < dt)
        d_t += cycle_secs;
      else {
        print(count);
        d_t = 0;
        count+=dt;
      }

      t_secs += cycle_secs;
      rt -= cycle_secs;
    }
    auto phys_alloc = top->t_alloc_mem;
    auto virt_alloc = top->v_alloc_mem;

    phys_alloc->t = virt_alloc->t = 'L';
    phys_alloc->pid = virt_alloc->pid = -1;

    write_phys(phys_alloc->i, phys_alloc->i + phys_alloc->s, -1);
    write_virt(virt_alloc->i, virt_alloc->i + virt_alloc->s, -1);

    p_queue.pop();
    delete top;
  }
}

/*Limite inferior para o Quick Fit*/
#define LI 4

/*First Fit*/
mem_node* ff_aloc(int size) {
    mem_node *node, *temp;

    /*Percorre a memória virtual*/
    for(node = v_mem_h->n; node != v_mem_h; node = node->n) {
        if (node->t == 'L' && node->s >= size){
            /*Caso tem espaço livre, dividimos entre dois casos: se sobrou espaço livre ou se o espaço livre foi exato*/
            if(node->s > size) {
                temp = new mem_node('L', node->i + size, node->s - size, node->n, node);

                node->t = 'P';
                node->s = size;
                node->n = temp;

                return (node);
            }
            else{
                node->t = 'P';
                return (node);
            }
        }
    }
    return(nullptr);
}

/*Next Fit*/

mem_node* nf_aloc(int size) {
    static mem_node *v_last = v_mem_h;
    mem_node *node, *temp;

    /*Pepois percorre a memória virtual começando por last*/
    for(node = v_last->n; node != v_last; node = node->n) {
        if (node->t == 'L' && node->s >= size){
            /*Caso tem espaço livre, dividimos entre dois casos: se sobrou espaço livre ou se o espaço livre foi exato*/
            if(node->s > size) {
                temp = new mem_node('L', node->i + size, node->s - size, node->n, node);

                node->t = 'P';
                node->s = size;
                node->n = temp;

                v_last = node;
                return (node);
            }
            else{
                node->t = 'P';
                v_last = node;
                return (node);
            }
        }
    }
    return(nullptr);
}

mem_node* qf_aloc(int size) {
    mem_node *node, *temp;

    node = v_search(size);
    if(node != nullptr) {
        temp = v_mem_h->p;
        temp->n = node;
        v_mem_h->p = node;
        node->n = v_mem_h;
        node->p = temp;
        return (node);
    }

    return(nullptr);
}

mem_node* v_search(int size) {
    size_node *node;
    mem_node *temp;
    for (node = v_size_h->n; node != v_size_h; node = node->n){
        /*Tem algum espaço livre disponível*/
        if(node->s >= size && node->f != nullptr) {
            temp = node->f;
            temp->t = 'P';
            if(temp->n != nullptr)
                temp->n->p = nullptr;
            node->f = temp->n;
            return (temp);
        }
    }
    return(nullptr);
}

void ff_nf_free(mem_node *node) {
    mem_node *temp;


    /*Caso específico em que o processo está entre dois processos livre,
    que após liberar o espaço vai juntar os 3*/
    if(node->p->t == 'L' && node->n->t == 'L'){
        temp = node->p;
        temp->s += node->s + node->n->s;
        temp->n = node->n->n;
        node->n->n->p = temp;
        delete node->n;
        delete node;
    }
    /*Caso específico onde apenas o bloco anterior estava livre*/
    else if(node->p->t == 'L'){
        temp = node->p;
        temp->s += node->s;
        temp->n = node->n;
        node->n->p = temp;
        delete node;
    }
    /*Caso específico onde apenas o bloco posterior estava livre*/
    else if(node->n->t == 'L'){
        temp = node->n;
        node->s += temp->s;
        node->n = temp->n;
        temp->n->p = node;
        node->t = 'L';
        delete temp;
    }
    /*Caso os dois blocos adjacentes estejam ocupados, apenas "libera" o atual*/
    else
        node->t = 'L';

    return;
}

void qf_free(mem_node *node){
    size_node *s_temp;
    /*Acha a lista de blocos do tamanho de node*/
    for (s_temp = v_size_h; s_temp->s != node->s; s_temp= s_temp->n);

    /*Vai colocar o bloco de volta à lista de blocos livres*/
    node->t = 'L';
    node->n = s_temp->f;
    s_temp->f->p = node;
    s_temp->f = node;

}

