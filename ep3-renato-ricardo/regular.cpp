#include "regular.hpp"

#include <cstdio>
#include <ctime>
#include <string>

Regular::Regular(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), content_(), sizeb_(0) {}
Regular::~Regular(void) {}

int Regular::Size(void) const { return sizeb_; }
bool Regular::IsDirectory(void) const { return false; }

const std::string& Regular::ReadContent(FILE *stream) {
  const char *c_str = content_.c_str();

  fputs(c_str, stream);
  delete[] c_str;

  return content_;
}

void Regular::WriteContent(const std::string &data) {
  content_.assign(data);
  sizeb_ = content_.length() + 1;
}
