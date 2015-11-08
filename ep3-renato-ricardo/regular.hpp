#ifndef _REGULAR_HPP
#define _REGULAR_HPP

#include <cstdio>
#include <ctime>
#include <string>

#include "file.hpp"

class Regular : File {
  public:
    Regular(const std::string &name, time_t t_current);
    ~Regular(void);

    int GetSize(void) const override;
    bool IsDirectory(void) const override;

    const std::string& ReadContent(FILE *stream);
    void WriteContent(const std::string& data);

  private:
    std::string content_;
    int sizeb_;
};

#endif /* _REGULAR_HPP */
