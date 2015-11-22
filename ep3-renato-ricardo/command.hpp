#ifndef _COMMAND_HPP
#define _COMMAND_HPP

#include <string>
#include <vector>

#include "directory.hpp"

namespace Command {
  using ArgsTable = const std::vector<std::string>&;

  void mount(ArgsTable args);
  void cp(ArgsTable args);
  void mkdir(ArgsTable args);
  void rmdir(ArgsTable args);
  void cat(ArgsTable args);
  void touch(ArgsTable args);
  void rm(ArgsTable args);
  void ls(ArgsTable args);
  void find(ArgsTable args);
  void df(ArgsTable args);
  void umount(ArgsTable args);
  void sai(ArgsTable);

  using Function = void (*)(ArgsTable);
  extern const Function kFunctions[];
}

#endif
