#include "mem_mgr.hpp"
#include "utils.hpp"

/*First Fit*/
mem_node* ff(int size) {
  mem_node *node, *temp;
  /*Primeiro percorre a memória principal*/
  for(node = t_mem_h->n; node != t_mem_h; node = node->n) {
    if (node->s >= size){
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
  /*Depois percorre a memória virtual*/
  for(node = v_mem_h->n; node != v_mem_h; node = node->n) {
    if (node->s >= size){
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
}
