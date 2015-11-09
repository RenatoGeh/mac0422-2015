#include "utils.hpp"

#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "command.hpp"

namespace Utils {
  /* -- Constants -- */

  const char *kPrompt = "[ep3]: ";

  /* 100MB */
  const int kSystemSize = 100000000;
  /* 4KB */
  const int kBlockSize = 4000;

  const char *kCommands[] = {"mount", "cp", "mkdir", "rmdir", "cat", "touch", "rm", "ls", "find",
    "df", "umount", "sai"};

  /* -- Functions -- */

  std::vector<std::string>& Tokenize(char *line) {
    static std::vector<std::string> mapper;

    mapper.clear();

    char *tok = strtok(line, " ");
    while (tok != NULL) {
      mapper.push_back(std::string(tok));
      tok = strtok(NULL, " ");
    }

    return mapper;
  }

  std::function<void(const std::vector<std::string>&)>* CommandToFunction(const std::string& cmd) {
    static bool init = false;
    static std::map<std::string, std::function<void(const std::vector<std::string>&)>> mapper;

    if (!init) {
      int s = sizeof(kCommands)/sizeof(*kCommands);
      for (int i = 0; i < s; ++i)
        mapper.insert(std::pair<std::string, std::function<void(const std::vector<std::string>&)>>(
              kCommands[i], Command::kFunctions[i]));
    }

    std::function<void(const std::vector<std::string>&)> *val = nullptr;
    try {
      *val = mapper.at(cmd);
    } catch(const std::out_of_range &exc) {
      fprintf(stderr, "%s, command not found\n", cmd.c_str());
      return nullptr;
    }

    return val;
  }
}
