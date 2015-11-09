#ifndef _FILE_HPP
#define _FILE_HPP

#include <ctime>
#include <string>

class File {
  public:
    File(const std::string &name, time_t t_create, time_t t_modify, time_t t_access);
    ~File(void);

    const std::string& Name(void) const { return name_; }
    void Rename(const std::string& name) { name_.assign(name); }

    time_t CreationTime(void) { return t_create_; }
    void RefreshCreationTime(void) { time(&t_create_); }

    time_t ModifiedTime(void) { return t_modify_; }
    void RefreshModifiedTime(void) { time(&t_modify_); }

    time_t AccessedTime(void) { return t_access_; }
    void RefreshAccessedTime(void) { time(&t_access_); }

    virtual int Size(void) const = 0;
    virtual bool IsDirectory(void) const = 0;

    bool operator<(const File &rval) { return name_.compare(rval.Name())<0; }
    bool operator==(const File &rval) { return !name_.compare(rval.Name()); }
    bool operator>(const File &rval) { return name_.compare(rval.Name())>0; }

  private:
    std::string name_;

    time_t t_create_;
    time_t t_modify_;
    time_t t_access_;
};

#endif /* _FILE_HPP */
