#include "utils.hpp"

/*First Fit*/
int ff(int size) {
	int i, j, fit;
	/*Percorre a memória total*/
	for(i = 0; i < t_size; i++) {
		if(!memory[i]) {
			/*Checa se o tamanho livre é o suficiente*/
			for(j = 1, fit = true; j < size; j++){
				if(t_memory[i+j]) {
					fit = false;
					break;
				}
			}
			/*Se couber, marca que esta ocupado*/
			if(fit){
				for(j = i; j-i< size; j++)
					memory[j] = 1;
				return(i);
			}
		}
	}
	/*Percorre a memória virtual*/
	for(i = 0; i < v_size; i++) {
		if(!memory[i]) {
			/*Checa se o tamanho livre é o suficiente*/
			for(j = 1, fit = true; j < size; j++) {
				if(v_memory[i+j]) {
					fit = false;
					break;
				}
			}
			/*Se couber, marca que esta ocupado*/
			if(fit){
				for(j = i; j-i< size; j++)
					memory[j] = 1;
				return(i);
			}
		}
	}
}