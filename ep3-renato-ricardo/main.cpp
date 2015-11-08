#include <cstdio>
#include <ctime>
#include <cstring>

#include <string>
#include <vector>
#include <functional>

#include <readline/readline.h>
#include <readline/history.h>

#include "utils.hpp"

int main(int argc, char *argv[]) {
  using_history();

  char *cmd_line;
  do {
    cmd_line = readline(Utils::kPrompt);
    add_history(cmd_line);
    std::vector<std::string>& arg_table = Utils::Tokenize(cmd_line);

    std::function<void(const std::vector<std::string>&)> *fn = Utils::CommandToFunction(cmd_line);
    if (fn != nullptr)
      (*fn)(arg_table);

    delete cmd_line;
  } while (1);

  return 0;
}
