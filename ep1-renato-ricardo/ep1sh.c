/* ep1sh.c 
 * MAC0422 - 2015
 *
 * Renato Geh      - 8536030
 * Ricardo Fonseca - 8536131
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Tamanho maximo do string path. */
#define M_PATH_SIZE 500

/* Maximo numero de argumentos. */
#define M_ARGS 20

/* Home do SO (normalmente ~). */
char *HOME;

/* Tabela de comandos embutidos. */
char *rec_cmd[] = {
  "cd",
  "pwd",
  "exit",
};

/* Tabela de argumentos. */
char *args_table[M_ARGS];

/* String path interno. */
char path[M_PATH_SIZE];

/* Roda execve(cmd, argv, env) em um processo filho. */
int run_ext_cmd(const char *cmd, char *const argv[], char *const env[]);

/* Roda comandos embutidos. */
int run_cmd(int cmd_index, char *cmd[]);

/* Retorna posicao do comando embutido ou -1 se nao existe. */
int cmd_exists(char *cmd);

/* Extrai argumentos de uma string em args_table. */
void extract_args(char *line);

int main(int argc, char *argv[]) {
  char *cmd;
  char *env[] = {NULL};
  char prompt[M_PATH_SIZE+3];

  int i;

  /* Inicializacao. */
  HOME = getenv("HOME");
  getcwd(path, sizeof(path)/sizeof(path[0])); 
  cmd = NULL;
  for (i=0;i<M_ARGS;++i)
    args_table[i] = NULL;
  using_history();

  do {
    sprintf(prompt, "[%s] ", path);
  
    cmd = readline(prompt);  
    
    add_history(cmd);
    extract_args(cmd);

    if ((i=cmd_exists(args_table[0])) >= 0)
      run_cmd(i, args_table);       
    else
      run_ext_cmd(args_table[0], args_table, env);

    /* Iterative cleanup. */
    free(cmd);
    for (i=0;args_table[i] != NULL;++i) {
      free(args_table[i]);
      args_table[i] = NULL;
    }
  } while (1);

  /* Considera exit como o unico jeito de sair do shell, entao nao ha cleanup aqui. */

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
  /* Espera o processo child acabar. */
  waitpid(ch_id, NULL, 0);

  return 0;
}

int run_cmd(int cmd_index, char *args[]) {
  switch(cmd_index) {
    case 0:
      /* cd */
      if (args[1][0] == '/')
        /* Vai para root. */
        strcpy(path, args[1]);
      else if (args[1][0] == '~')
        /* Vai para home. */
        strcpy(path, HOME);
      else if (args[1][0] == '.') {
        if (args[1][1] == '.') {
          /* Volta apenas UM diretorio acima. */
          char *c, *l = c = path+1;

          /* Acha o ultimo diretorio de path. */
          while (*c!='\0') {
            if (*c == '/')
              l = c;
            ++c;
          }

          /* Considera-se que path nunca vai ser vazio, no maximo "/". */
          *l = '\0';
        } else {
          /* args[1] do formato "./outro/diretorio" ou "." */
          if (!(args[1][1] == '\0' || (args[1][1] == '/' && args[1][2] == '\0')))
            strcat(path, args[1]+1);
        }
      } else 
        sprintf(path, "%s/%s", path, args[1]);
      return chdir(path);
    break;
    case 1:
      /* pwd */ {
        char pwd[M_PATH_SIZE];
        getcwd(pwd, M_PATH_SIZE);
        puts(pwd);
      }
    break;
    case 2:
      /* exit */
      exit(EXIT_SUCCESS);
    break;
  } 

  return 0;
}

int cmd_exists(char *cmd) {
  int size = sizeof(rec_cmd)/sizeof(rec_cmd[0]);
  int i;

  for (i=0;i<size;++i) 
    if (strcmp(cmd, rec_cmd[i]) == 0)
      return i; 

  return -1;
}

void extract_args(char *line) {
  char *token;
  int i_args = 0;  

  token = strtok(line, " ");
  while (token != NULL) {
    int len = strlen(token);
    free(args_table[i_args]);
    args_table[i_args] = (char*) malloc((len+1)*sizeof(char));
    strcpy(args_table[i_args], token); 
    token = strtok(NULL, " ");
    ++i_args;
  }
  args_table[i_args] = NULL;
}
