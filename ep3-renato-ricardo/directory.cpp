#include "directory.hpp"

#include <cstdio>
#include <ctime>
#include <string>
#include <map>

Directory::Directory(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), files_(), files_sizeb_(0) {}
Directory::~Directory(void) {}

int Directory::GetSize(void) const { return Utils::kBlockSize; }
bool Directory::IsDirectory(void) const { return true; }

void Directory::ListFiles(FILE *stream) {
  int *digits = new int[files_.size()];
  int max_digits = 0, i = 0;
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    int n = it->second->GetSize();
    for (digits[i] = 0; n; n /= 10, ++digits[i]);
    if (digits[i] > max_digits)
      max_digits = digits[i];
  }

  fprintf(stream, "total %d bytes\n", files_sizeb_);
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    File *it_val = it->second;
    char time_buffer[128];
    time_t modt_clone = it_val->GetModifyTime();
    strftime(time_buffer, sizeof(time_buffer)/sizeof(*time_buffer), "%c", localtime(&modt_clone));
    fprintf(stream, "[%c] %*d %s %s\n", it_val->IsDirectory()?'D':'R', max_digits-digits[i],
        it_val->GetSize(), time_buffer, it_val->GetName().c_str());
  }

  delete[] digits;
}

void Directory::InsertFile(File *f) {
  files_.insert(std::pair<std::string, File*>(f->GetName(), f));
  files_sizeb_ += f->GetSize();
}

void Directory::RemoveFile(const std::string &name) {
  files_sizeb_ -= files_.at(name)->GetSize();
  files_.erase(name);
}

File* Directory::FindFile(const std::string &name) {
  return files_.at(name);
}
