#include "directory.hpp"

#include <cstdio>
#include <ctime>
#include <string>
#include <forward_list>
#include <iterator>

Directory::Directory(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), files_(), n_files_(0), files_sizeb_(0) {}
Directory::~Directory(void) {}

int Directory::Size(void) const { return Utils::kBlockSize; }
bool Directory::IsDirectory(void) const { return true; }

void Directory::ListFiles(FILE *stream) {
  files_.sort([](File *lval, File *rval){ return *lval < *rval; });

  int *digits = new int[n_files_];
  int max_digits = 0, i = 0;
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    int n = (*it)->Size();
    for (digits[i] = 0; n; n /= 10, ++digits[i]);
    if (digits[i] > max_digits)
      max_digits = digits[i];
  }

  fprintf(stream, "%d files with a total of %d bytes\n", n_files_, files_sizeb_);
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    File *it_val = *it;
    char time_buffer[128];
    time_t modt_clone = it_val->ModifiedTime();
    strftime(time_buffer, sizeof(time_buffer)/sizeof(*time_buffer), "%c", localtime(&modt_clone));
    fprintf(stream, "[%c] %*d %s %s\n", it_val->IsDirectory()?'D':'R', max_digits-digits[i],
        it_val->Size(), time_buffer, it_val->Name().c_str());
  }

  delete[] digits;
}

void Directory::InsertFile(File *f) {
  files_.push_front(f);
  files_sizeb_ += f->Size();
  ++n_files_;
}

void Directory::RemoveFile(const std::string &name) {
  auto to_remove = files_.end();
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    File *it_val = *it;
    if (!name.compare(it_val->Name())) {
      files_sizeb_ -= it_val->Size();
      to_remove = std::prev(it);
      break;
    }
  }
  files_.erase_after(to_remove);
  --n_files_;
}

File* Directory::FindFile(const std::string &name) {
  for (auto it = files_.begin(); it != files_.end(); ++it)
    if (!name.compare((*it)->Name()))
      return *it;
  return nullptr;
}
