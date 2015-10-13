#include "mem_mgr.hpp"

#include <cmath>

#include "utils.hpp"

enum MEM_ALG {
  FF = 1,
  NF,
  QF,
};

int mem_alg = MEM_ALG::FF;
int mem_status_dt = -1;

inline void set_mem_mgr(int opt) { opt = mem_alg; }

void run_mem_mgr(int dt) {
  mem_status_dt = dt;

  switch(mem_alg) {
    case MEM_ALG::FF:
      /* ff_aloc(s); */
      break;
    case MEM_ALG::NF:
      /* nf_aloc(s); */
      break;
    case MEM_ALG::QF:
      /* qf_aloc(s); */
      break;
    default:
      fputs("Erro.\n", stderr);
  }
}

/*Limite inferior para o Quick Fit*/
#define LI 3

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
    if(node != nullptr)
        temp = v_mem_h->p;
        temp->n = node;
        v_mem_h->p = node;
        node->n = v_mem_h;
        node->p = temp;
        return (node);

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

