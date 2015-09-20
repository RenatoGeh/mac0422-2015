#ifndef _MEM_MGR_
#define _MEM_MGR_

#define M_ARGS 10

/* Tabela de argumentos. */
char *args_table[M_ARGS];

/* Extrai comando e argumentos de uma linha. */
void extract_args(char *line);

#endif /* _MEM_MGR_ */
