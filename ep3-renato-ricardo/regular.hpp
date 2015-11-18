#ifndef _REGULAR_HPP
#define _REGULAR_HPP

#include <cstdio>
#include <ctime>
#include <string>

#include "file.hpp"
#include "block.hpp"

class Regular : public File {
  public:
    Regular(const std::string &name, time_t t_current);
    ~Regular(void);

    long int Size(void) const override;
    bool IsDirectory(void) const override;

    std::string ReadContent(FILE *stream);
    void WriteContent(const std::string& data);

  private:
    Block *block_head_;
    long int sizeb_;
};

#endif /* _REGULAR_HPP */
