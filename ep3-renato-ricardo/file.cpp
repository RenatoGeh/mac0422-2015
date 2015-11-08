#include <ctime>
#include "file.hpp"

File::File(const std::string &name, time_t t_create, time_t t_modify, time_t t_access) :
  name_(name), t_create_(t_create), t_modify_(t_modify), t_access_(t_access) {}
File::~File(void) {}