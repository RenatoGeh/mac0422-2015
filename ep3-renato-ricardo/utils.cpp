#include "utils.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>

#include <string>
#include <vector>
#include <map>

#include "command.hpp"
#include "directory.hpp"

namespace Utils {
  /* -- Constants -- */

  const char *kPrompt = "[ep3]: ";

  /* 100MB */
  const int kSystemSize = 100000000;
  /* 4KB */
  const int kBlockSize = 4000;

  const char *kCommands[] = {"mount", "cp", "mkdir", "rmdir", "cat", "touch", "rm", "ls", "find",
    "df", "umount", "sai"};

  /* Root ("/") */
  const Directory kRoot("/", Time::Get()); 

  /* -- Functions -- */

  std::vector<std::string>& Tokenize(char *line) {
    static std::vector<std::string> a_table;

    a_table.clear();

    char *tok = strtok(line, " ");
    while (tok != NULL) {
      a_table.push_back(std::string(tok));
      tok = strtok(NULL, " ");
    }

    return a_table;
  }

  using fn_args = void (*) (const std::vector<std::string>&);

  fn_args CommandToFunction(const std::string& cmd) {
    static bool init = false;
    static std::map<std::string, fn_args> mapper;

    if (!init) {
      int s = sizeof(kCommands)/sizeof(*kCommands);
      for (int i = 0; i < s; ++i)
        mapper.insert(std::pair<std::string, fn_args>(kCommands[i], Command::kFunctions[i]));
      init = true;
    }

    fn_args val = nullptr;
    try {
      val = mapper.at(cmd);
    } catch(const std::out_of_range &exc) {
      fprintf(stderr, "%s, command not found\n", cmd.c_str());
      return nullptr;
    }

    return val;
  }

  namespace Time {
    time_t Get(void) { return time(NULL); }
  }
}
