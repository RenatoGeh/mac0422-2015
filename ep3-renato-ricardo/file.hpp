#ifndef _FILE_HPP
#define _FILE_HPP

#include <ctime>
#include <string>

class File {
  public:
    File(const std::string &name, time_t t_create, time_t t_modify, time_t t_access);
    ~File(void);

    const std::string& GetName(void) { return name_; }
    void Rename(const std::string& name) { name_.assign(name); }

    time_t GetCreateTime(void) { return t_create_; }
    void RefreshCreateTime(void) { time(&t_create_); }

    time_t GetModifyTime(void) { return t_modify_; }
    void RefreshModifyTime(void) { time(&t_modify_); }

    time_t GetAccessTime(void) { return t_access_; }
    void RefreshAccessTime(void) { time(&t_access_); }

    virtual int GetSize(void) const = 0;
    virtual bool IsDirectory(void) const = 0;

  private:
    std::string name_;

    time_t t_create_;
    time_t t_modify_;
    time_t t_access_;
};

#endif /* _FILE_HPP */
