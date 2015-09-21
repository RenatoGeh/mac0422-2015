#include "mem_mgr.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#define PROMPT "[ep2]: "

int main(int argc, char *argv[]) {
  char *cmd;
  
  using_history();

  while (true) {
    cmd = readline(PROMPT);

    add_history(cmd);
    extract_args(cmd);

    free(cmd);
    for (int i=0;args_table[i]!=NULL;++i) {
      free(args_table[i]);
      args_table[i] = NULL;
    }
  }

  return 0;
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
