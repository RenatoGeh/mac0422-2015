#include "directory.hpp"

#include <cstdio>
#include <ctime>
#include <string>
#include <forward_list>
#include <iterator>

#include "utils.hpp"
#include "block.hpp"

Directory::Directory(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), files_(), n_files_(0), sizeb_(0), files_sizeb_(0),
  block_head_(nullptr) {
    block_head_ = Utils::BlockManager::NextAvailable();
    if (block_head_ == nullptr) {
      Utils::Exception::NoMemory no_mem;
      fprintf(stderr, "Could not create directory %s.\n%s\nReason: no blocks left.",
          name.c_str(), no_mem.message());
      throw no_mem;
    }

    long int size_str = name.length();
    long int n_blocks = Utils::BytesToBlocks(size_str);

    if (n_blocks > Utils::BlockManager::Available()) {
      Utils::Exception::NoMemory no_mem;
      fprintf(stderr, "Could not create directory %s.\n%s\n"
          "Reason: name too long, no blocks left.", name.c_str(), no_mem.message());
      throw no_mem;
    }

    std::string head, tail(name);
    Block *prev_block = block_head_;

    for (long int i = 0; i < n_blocks; ++i) {
      head.assign(tail.substr(0, Utils::kBlockSize));
      try {
        tail.assign(tail.substr(Utils::kBlockSize+1, tail.length()));
      } catch(std::exception &oor) {
        /* Nunca deveria passar por aqui. */
        fprintf(stderr, "Failed to create directory name.\nException OutOfRange: %s\n"
            "Reason: Unknown.", oor.what());
        throw oor;
      }

      Block *b = Utils::BlockManager::NextAvailable();
      b->Write(head);
      prev_block->SetNext(b);
      b->SetPrev(prev_block);
    }
}
Directory::~Directory(void) {
  if (block_head_ != nullptr)
    Utils::BlockManager::Free(block_head_);
}

long int Directory::Size(void) const { return sizeb_; }

bool Directory::IsDirectory(void) const { return true; }

void Directory::ListFiles(FILE *stream) {
  files_.sort([](File *lval, File *rval){ return *lval < *rval; });

  int *digits = new int[n_files_];
  int max_digits = 0, i = 0;
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    long int n = (*it)->Size();
    for (digits[i] = 0; n; n /= 10, ++digits[i]);
    if (digits[i] > max_digits)
      max_digits = digits[i];
  }

  fprintf(stream, "%d files with a total of %ld bytes\n", n_files_, files_sizeb_);
  for (auto it = files_.begin(); it != files_.end(); ++it) {
    File *it_val = *it;
    char time_buffer[128];
    time_t modt_clone = it_val->ModifiedTime();
    strftime(time_buffer, sizeof(time_buffer)/sizeof(*time_buffer), "%c", localtime(&modt_clone));
    fprintf(stream, "[%c] %*ld %s %s\n", it_val->IsDirectory()?'D':'R', max_digits-digits[i],
        it_val->Size(), time_buffer, it_val->Name().c_str());
  }

  delete[] digits;
}

void Directory::InsertFile(File *f) {
  int size = -1;
  try {
    size = WriteName(f->Name());
  } catch (Utils::Exception::NoMemory &no_mem) {
    fprintf(stderr, "Could not create file %s.\n%s\nReason: file name too long.\n", 
        f->Name().c_str(), no_mem.message());
    return;
  }

  sizeb_ += size;

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

long int Directory::WriteName(const std::string &new_name) {
  long int size_str = new_name.length();
  Block *current_ptr = *Utils::BlockManager::End(block_head_);

  long int diff = Utils::kBlockSize - current_ptr->Bytes();
  if (diff > 0) {
    current_ptr->Append(new_name.substr(0, diff));
    return size_str;
  }

  long int n_blocks = Utils::BytesToBlocks(size_str-diff);

  if (n_blocks > Utils::BlockManager::Available())
    throw Utils::Exception::NoMemory();

  std::string mod_name(new_name.substr(diff, new_name.length()));
  for (long int i = 0; i < n_blocks; ++i) {
    Block *b = Utils::BlockManager::NextAvailable();
    std::string data(mod_name.substr(i*Utils::kBlockSize, Utils::kBlockSize));

    b->Write(data);
    current_ptr->SetNext(b);
    b->SetPrev(current_ptr);
    current_ptr = b;
  }

  return size_str;
}
