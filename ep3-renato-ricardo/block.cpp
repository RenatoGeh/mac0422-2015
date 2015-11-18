#include "block.hpp"

#include <string>

Block::Block(long int i_block, const std::string &content) :
  next_(-1), prev_(-1), index_(i_block), content_(content) {}
Block::~Block(void) {}

void Block::Write(const std::string &data) {
  content_.assign(data);
}

void Block::Append(const std::string &data) {
  content_.append(data);
}

const std::string& Block::Read(void) {
  return content_;
}
