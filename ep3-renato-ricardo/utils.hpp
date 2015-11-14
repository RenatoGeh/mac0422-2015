#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <ctime>

#include <string>
#include <vector>

#include "directory.hpp"
#include "block.hpp"

/* Forward declarations. */
class Directory;

namespace Utils {
  /* -- Constants -- */

  /* Prompt. */
  extern const char *kPrompt;

  /* Tamanho maximo do sistema de arquivos. */
  extern const long int kSystemSize;

  /* Tamanho em bytes de um bloco. */
  extern const long int kBlockSize;

  /* Numero de blocos no FAT. */
  extern const long int kNumBlocks;

  /* Possiveis comandos. */
  extern const char *kCommands[];

  /* Diretorio root. */
  extern const Directory kRoot;

  /* -- Functions -- */

  /* Tokeniza uma linha. */
  std::vector<std::string>& Tokenize(char *line);

  /* Mapeia um comando std::string -> int. */
  void (*CommandToFunction(const std::string& cmd)) (const std::vector<std::string>&);

  namespace Time {
    time_t Get(void);
  }

  namespace BlockManager {
    /* FAT Table. */
    /* Constant expression: kNumBlocks -> size. */
    extern Block* MemoryTable[25000];
    /* End of FAT. */
    extern const long int kEnd;

    class Iterator {
      public:
        Iterator(long int index);
        ~Iterator(void);

        Iterator& operator=(long int index);
        Iterator& operator=(const Iterator &it);

        Iterator& operator++(void);
        Iterator& operator--(void);

        friend bool operator==(const Iterator &lval, const Iterator &rval);
        friend bool operator==(const Iterator &lval, long int rval);

        Block* operator*(void) { return MemoryTable[index_]; }

        bool Ended() const { return index_<0 || index_>= kNumBlocks; }
        long int Index(void) const { return index_; }
      private:
        long int index_;
        long int next_;
        long int prev_;

        void Refresh(void) {
          Block *b = MemoryTable[index_];
          next_ = b->Next();
          prev_ = b->Prev();
        }
    };

    inline bool operator==(const Iterator &lval, const Iterator &rval) {
      return lval.index_ == rval.index_;
    }

    inline bool operator==(const Iterator &lval, long int rval) {
      return lval.index_ == rval;
    }

    /* Begin of a FAT list. */
    Iterator Begin(Block *b);

    /* Encontra proximo bloco livre. */
    Block* NextAvailable(void);
    /* Libera o bloco do indice dado e todos seus subsequentes. */
    void Free(long int i_block);
  }
}

#endif /* _UTILS_HPP */
