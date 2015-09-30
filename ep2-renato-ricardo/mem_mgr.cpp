#include "mem_mgr.hpp"
#include "utils.hpp"
#include "mem_mgr.hpp"
#include <cmath.h>

/*Limite inferior para o Quick Fit*/
#define LI 3

/*First Fit*/
mem_node* ff_aloc(int size) {
    mem_node *node, *temp;
    /*Primeiro percorre a memória principal*/
    for(node = t_mem_h->n; node != t_mem_h; node = node->n) {
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
    /*Depois percorre a memória virtual*/
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

mem_node* nf_aloc(int size){
    static mem_node *t_last = t_mem_h;
    static mem_node *v_last = v_mem_h;
    mem_node *node, *temp;
    /*Primeiro percorre a memória principal começando por last*/
    for(node = t_last->n; node != t_last; node = node->n) {
        if (node->t == 'L' && node->s >= size){
            /*Caso tem espaço livre, dividimos entre dois casos: se sobrou espaço livre ou se o espaço livre foi exato*/
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
                v_last = node;
                return (node);
            }
        }
    }
    /*Depois percorre a memória virtual começando por last*/
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

mem_node* qf_aloc(int size){
    mem_node *node;

    node = t_search(size);
    if(node != nullptr)
        return (node);
    
    node = v_search(size);
    if(node != nullptr)
        return (node);
    
    return(nullptr);
}

mem_node* t_search(int size){
    size_node *node;
    mem_node *temp;
    for (node = t_size_h->n; node != t_size_h; node = node->n){
        /*Tem algum espaço livre disponível*/
        if(node->s >= size && node->f != nullptr) {
            temp = node->f;
            temp->t = 'P';
            if(temp->n != nullptr)
                temp->n->p = node;
            node->f = temp->n;
            temp->n = temp->p = nullptr;
            return (temp);
        }
    } 
    return(nullptr);
}

mem_node* v_search(int size){
    size_node *node;
    mem_node *temp;
    for (node = v_size_h->n; node != v_size_h; node = node->n){
        /*Tem algum espaço livre disponível*/
        if(node->s >= size && node->f != nullptr) {
            temp = node->f;
            temp->t = 'P';
            if(temp->n != nullptr)
                temp->n->p = node;
            node->f = temp->n;
            temp->n = temp->p = nullptr;
            return (temp);
        }
    } 
    return(nullptr);
}
