#include "regular.hpp"

#include <cstdio>
#include <ctime>
#include <string>

#include "utils.hpp"
#include "block.hpp"

Regular::Regular(const std::string &name, time_t t_current) :
  File(name, t_current, t_current, t_current), block_head_(nullptr), sizeb_(0) {}
Regular::~Regular(void) {
  if (block_head_ != nullptr)
    Utils::BlockManager::Free(block_head_->Index());
}

long int Regular::Size(void) const { return sizeb_ + Name().length(); }
bool Regular::IsDirectory(void) const { return false; }

std::string Regular::ReadContent(FILE *stream) {
  std::string content;

  for (auto it = Utils::BlockManager::Begin(block_head_); !it.Ended(); ++it)
    content.append((*it)->Read());

  return content;
}

/* Considera que o arquivo inteiro sera data. */
void Regular::WriteContent(const std::string &data) {
  /* TODO: Write. */
}
