#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <ctime>

#include <string>
#include <vector>

#include "directory.hpp"

/* Forward declarations. */
class Directory;

namespace Utils {
  /* -- Constants -- */

  /* Prompt. */
  extern const char *kPrompt;

  /* Tamanho maximo do sistema de arquivos. */
  extern const int kSystemSize;

  /* Tamanho em bytes de um bloco. */
  extern const int kBlockSize;

  /* Possiveis comandos. */
  extern const char *kCommands[];

  /* Diretorio root. */
  extern const Directory kRoot;

  /* -- Functions -- */

  /* Tokeniza uma linha. */
  std::vector<std::string>& Tokenize(char *line);

  /* Mapeia um comando std::string -> int. */
  void (*CommandToFunction(const std::string& cmd)) (const std::vector<std::string>&);

  namespace Time {
    time_t Get(void);
  }
}

#endif /* _UTILS_HPP */
