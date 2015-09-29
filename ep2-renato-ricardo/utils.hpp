#ifndef _UTILS_
#define _UTILS_

struct Mem_node{
	char type; /*P se for um processo, L se for livre*/
	Mem_node *ini; /*Endereço de início da memória*/
	int size; /*Tamanho da memória*/
	Mem_node *next; /*Próximo chunk de memória*/
};



int mem_size;

int *memory;



#endif /* _UTILS_ */
