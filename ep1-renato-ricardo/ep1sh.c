#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

/* Tamanho maximo do string path. */
#define M_PATH_SIZE 500

/* Maximo numero de argumentos. */
#define M_ARGS 20

char *rec_cmd[] = {
  "cd",
  "pwd",
}

/* String path interno. */
char path[M_PATH_SIZE];

/* Roda execve(cmd, argv, env) em um processo filho. */
int run_ext_cmd(const char *cmd, char *const argv[], char *const env[]);

/* Roda comandos embutidos. */
int run_cmd(char *cmd);

/* Retorna posicao do comando embutido ou -1 se nao existe. */
int cmd_exists(char *cmd);

int main(int argc, char *args[]) {
  char *cmd;
  char *args[M_ARGS];
  char *env[] = {NULL};
  char prompt[M_PATH_SIZE+3];
  int res_proc;

  int i;

  /* Inicializacao. */
  strcpy(path, getenv("HOME")); 
  cmd = NULL;
  for (i=0;i<M_ARGS;++i)
    args[i] = NULL;

  do {
    sprintf(prompt, "[%s] ", path);
  
    cmd = readline(prompt);  

    if ((i=cmd_exists(cmd)))
      res_proc = run_cmd(i, cmd+1);       
    else
      res_proc = run_cmd(cmd[0], cmd+1, env);

  } while (strcmp(cmd, "exit"));

  /* Cleanup. */
  free(cmd);

  return 0;
}

int run_ext_cmd(const char *cmd, char *const argv[], char *const env[]) {
  pid_t ch_id;
  
  if ((ch_id = fork()) == 0) {
    /* Is child. */
    execve(cmd, argv, env);     
  } else if(ch_id == -1) {
    /* Error. */
    printf("Erro ao executar [%s].\n", cmd);
    return -1;
  }

  /* Is parent. */
  return 0;
}

int run_cmd(int cmd_index, char* args) {
  switch(cmd_index) {
    case 0:
      /* cd */
      
    break;
    case 1:
      /* pwd */
    break;
  } 

  return 0;
}

int cmd_exists(char *cmd) {
  int size = sizeof(rec_cmd);
  int i;

  for (i=0;i<size;++i)
    if (strcmp(cmd, rec_cmd[i]) == 0)
      return i;

  return -1;
}
