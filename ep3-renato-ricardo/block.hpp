#ifndef _BLOCK_HPP
#define _BLOCK_HPP

#include <string>

class Block {
  public:
    Block(long int i_block, const std::string &content);
    ~Block(void);

    long int Next(void) const { return next_; }
    long int Prev(void) const { return prev_; }

    void SetNext(long int next) { next_ = next; }
    void SetPrev(long int prev) { prev_ = prev; }

    void SetNext(Block *next_block) { next_ = next_block->Index(); }
    void SetPrev(Block *prev_block) { prev_ = prev_block->Index(); }

    void Write(const std::string &data);
    void Append(const std::string &data);
    const std::string& Read(void);

    long int Bytes(void) { return content_.length(); }

    long int Index(void) const { return index_; }
  private:
    long int next_;
    long int prev_;

    long int index_;

    std::string content_;
};

#endif /* _BLOCK_HPP */
