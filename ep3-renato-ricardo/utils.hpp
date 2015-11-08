#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <string>
#include <vector>
#include <functional>

namespace Utils {
  /* -- Constants -- */

  /* Prompt. */
  extern const char *kPrompt;

  /* Tamanho em bytes de um bloco. */
  extern const int kBlockSize;

  /* Possíveis comandos. */
  extern const char *kCommands[];

  /* -- Functions -- */

  /* Tokeniza uma linha. */
  std::vector<std::string>& Tokenize(char *line);

  /* Mapeia um comando std::string -> int. */
  std::function<void(const std::vector<std::string>&)>* CommandToFunction(const std::string& cmd);
}

#endif
