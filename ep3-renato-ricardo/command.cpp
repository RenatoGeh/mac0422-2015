#include "command.hpp"

#include <string>
#include <vector>

#include "utils.hpp"
#include "directory.hpp"
#include "stream.hpp"

namespace Command {
  using ArgsTable = const std::vector<std::string>&;

  void mount(ArgsTable args) {
    Stream::Input::ReadMeta();
  }

  void cp(ArgsTable args) {

  }

  void mkdir(ArgsTable args) {
  }

  void rmdir(ArgsTable args) {

  }

  void cat(ArgsTable args) {

  }

  void touch(ArgsTable args) {

  }

  void rm(ArgsTable args) {

  }

  void ls(ArgsTable args) {
  }

  void find(ArgsTable args) {

  }

  void df(ArgsTable args) {

  }

  void umount(ArgsTable args) {

  }

  void sai(ArgsTable args) {
    exit(0);
  }

  using Function = void (*)(ArgsTable);
  const Function kFunctions[] = {mount, cp, mkdir, rmdir, cat, touch, rm, ls, find, df,
    umount, sai};
}
