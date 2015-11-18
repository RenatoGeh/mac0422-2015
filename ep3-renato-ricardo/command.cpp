#include "command.hpp"

#include <string>
#include <vector>

#include "utils.hpp"
#include "directory.hpp"

namespace Command {
  using ArgsTable = const std::vector<std::string>&;

  namespace {
    Directory &dir_ = Utils::kRoot;
  }

  Directory& Path(void) { return dir_; }
  void SetPath(Directory &dir) { dir_ = dir; }

  void mount(ArgsTable args) {

  }

  void cp(ArgsTable args) {

  }

  void mkdir(ArgsTable args) {
    dir_.InsertFile(new Directory(args.at(1), Utils::Time::Get()));
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
    dir_.ListFiles(stdout);
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
