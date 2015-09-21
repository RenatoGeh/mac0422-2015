#include "utils.hpp"

/*First Fit*/
int ff(int size) {
	int i, j, fit;
	/*Percorre a memória total*/
	for(i = 0; i + size < t_size; i+= t_memory[i]) {
		if(!memory[i]) {
			/*Checa se o tamanho livre é o suficiente*/
			for(j = 1, fit = true; j < size; j++){
				if(t_memory[i+j]) {
					i = i+j;
					fit = false;
					break;
				}
			}
			/*Se couber, marca que esta ocupado*/
			if(fit){
				memory[i] = size;
				return(i);
			}
		}
	}
	/*Percorre a memória virtual*/
	for(i = 0; i + size < v_size; i+= v_memory[i]) {
		if(!memory[i]) {
			/*Checa se o tamanho livre é o suficiente*/
			for(j = 1, fit = true; j < size; j++){
				if(v_memory[i+j]) {
					i = i+j;
					fit = false;
					break;
				}
			}
			/*Se couber, marca que esta ocupado*/
			if(fit){
				memory[i] = size;
				return(i);
			}
		}
	}
}