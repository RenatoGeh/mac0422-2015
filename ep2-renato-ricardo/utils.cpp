#include "utils.hpp"

void get_limits(void){
	t_ls = floor(log2(t_size));
	v_ls = floor(log2(v_size));
	return;
}

void create_qf(void){
	int i;
	size_node *node, *temp;
	get_limits();

	/*Cria a lista de rápido acesso para a memória total*/
	t_size_h = new size_node(0);
	t_size_h->f = t_size_h->n = t_size_h->p = nullptr;
	for (i = li, node = t_size_h->n; i <= t_ls; i++){
		temp = new size_node(exp2(i), nullptr, nullptr, node)
		node->n = temp;
		node = temp;
	}
	node->n = t_size_h;
	t_size_h->p = node;

	/*Cria a lista de rápido acesso para a memória virtual*/
	v_size_h = new size_node(0);
	v_size_h->f = v_size_h->n = v_size_h->p = nullptr;
	for (i = li, node = v_size_h->n; i <= v_ls; i++){
		temp = new size_node(exp2(i), nullptr, nullptr, node)
		node->n = temp;
		node = temp;
	}
	node->n = v_size_h;
	v_size_h->p = node;

	t_dis_mem(0, t_size);
	v_dis_mem(0, v_size);

	return;
}

void t_dis_mem(int pos, int size){
	int max, i, exp_li = exp2(li);
	max = size/4;
	/*Aproxima até o maior multiplo de 2 menor que max*/
	max = exp2(floor(log2(max)));
	if(max > exp_li){
		t_separate(pos, max);
		t_separate(pos+max, max);
		t_separate(pos+(2*max), max);
		t_dis_mem(pos+(3*max), size-3*max);
	}
	else {
		for(i = 0; i <= size-exp_li; i+=exp_li)
			t_separate(pos+i, exp_li)
	}
	return;
}


void v_dis_mem(int pos, int size){
	int max, i, exp_li = exp2(li);
	max = size/4;
	/*Aproxima até o maior multiplo de 2 menor que max*/
	max = exp2(floor(log2(max)));
	if(max > exp_li){
		v_separate(pos, max);
		v_separate(pos+max, max);
		v_separate(pos+(2*max), max);
		v_dis_mem(pos+(3*max), size-(3*max);
	}
	else {
		for(i = 0; i <= size-exp_li; i+=exp_li)
			v_separate(pos+i, exp_li)
	}
	return;
}

void t_separate(int pos, int size){
	size_node *node;
	mem_node *temp1, *temp2;
	for(node = t_size_h->n; node->s != size; node=node->n);
	for(temp1 = node->f; temp1 != nullptr; temp1 = temp1->n);
	temp2 = new mem_node('L', pos, size, nullptr, temp1);
	temp1->n = temp2;

	return
}

void v_separate(int pos, int size){
	size_node *node;
	mem_node *temp1, *temp2;
	for(node = v_size_h->n; node->s != size; node=node->n);
	for(temp1 = node->f; temp1 != nullptr; temp1 = temp1->n);
	temp2 = new mem_node('L', pos, size, nullptr, temp1);
	temp1->n = temp2;

	return
}



/* Utils. */
