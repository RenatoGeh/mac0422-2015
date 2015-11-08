#ifndef _DIRECTORY_HPP
#define _DIRECTORY_HPP

#include <cstdio>
#include <ctime>
#include <string>
#include <map>

#include "file.hpp"
#include "utils.hpp"

class Directory : File {
  public:
    Directory(const std::string &name, time_t t_current);
    ~Directory(void);

    int GetSize(void) const override;
    bool IsDirectory(void) const override;

    void ListFiles(FILE *stream);

    void InsertFile(File *f);
    void RemoveFile(const std::string &name);
    File* FindFile(const std::string &name);

  private:
    std::map<std::string, File*> files_;

    int files_sizeb_;
};

#endif /* _DIRECTORY_HPP */
