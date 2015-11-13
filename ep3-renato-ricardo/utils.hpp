#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <string>
#include <vector>

namespace Utils {
  /* -- Constants -- */

  /* Prompt. */
  extern const char *kPrompt;

  /* Tamanho maximo do sistema de arquivos. */
  extern const int kSystemSize;

  /* Tamanho em bytes de um bloco. */
  extern const int kBlockSize;

  /* Possíveis comandos. */
  extern const char *kCommands[];

  /* -- Functions -- */

  /* Tokeniza uma linha. */
  std::vector<std::string>& Tokenize(char *line);

  /* Mapeia um comando std::string -> int. */
  void (*CommandToFunction(const std::string& cmd)) (const std::vector<std::string>&);
}

#endif
