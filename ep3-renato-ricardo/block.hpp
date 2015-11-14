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

    void Write(const std::string &data);
    const std::string& Read(void);

    long int Index(void) const { return index_; }
  private:
    long int next_;
    long int prev_;

    long int index_;

    std::string content_;
};

#endif /* _BLOCK_HPP */
