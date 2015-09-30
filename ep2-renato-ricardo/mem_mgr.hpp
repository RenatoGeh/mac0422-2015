#ifndef _MEM_MGR_HPP
#define _MEM_MGR_HPP

#include "utils.hpp"

/*Algoritmo First Fit: Pega a primeira posição
livre da memória que caiba o processo de tamanho size*/
mem_node* ff_aloc(int size);

/*Algoritmo Next Fit: Pega a primeira posição
livre da memória que caiba o processo de tamanho size, porém a partir
da ultima posição verificada*/
mem_node* nf_aloc(int size);

/*Algoritmo Quick Fit:  Vai dividir a memória disponível em listas de
tamanhos predefinidos (multiplos de 2) e encontra o melhor espaço que
caiba o processo*/
mem_node* qf_aloc(int size);

/*Procura na memória total o bloco de tamanho mais adequado que caiba o processo de tamanho size*/
mem_node* t_search(int size);

/*Procura na memória virtual o bloco de tamanho mais adequado que caiba o processo de tamanho size*/
mem_node* v_search(int size);


#endif /*_MEM_MGR_HPP*/

