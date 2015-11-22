#include "regular.hpp"

#include <cstdio>
#include <ctime>
#include <string>

#include "utils.hpp"
#include "block.hpp"
#include "stream.hpp"

Regular::Regular(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), sizeb_(0) {}
Regular::~Regular(void) {
  if (block_head_ > 0)
    Utils::BlockManager::Free(block_head_);
}

long int Regular::Size(void) const { return sizeb_ + Name().length(); }
bool Regular::IsDirectory(void) const { return false; }

std::string Regular::ReadContent(FILE *stream) {
  std::string content;

  for (long int i=block_head_;i>0&&i<Utils::kNumBlocks;i=Utils::BlockManager::MemoryTable[i])
    content.append(Stream::Input::Read(i));

  return content;
}

/* Considera que o arquivo inteiro sera data. */
void Regular::WriteContent(const std::string &data) {
  /* TODO: Write. */
}
