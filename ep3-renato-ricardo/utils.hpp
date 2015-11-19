#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <ctime>

#include <string>
#include <vector>
#include <exception>

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

  /* Tamanho em bytes de um ponteiro. */
  extern const long int kPointerSize;
  extern const long int kPointerBytes;

  /* Tamanho em um byte. */
  extern const long int kByte;

  /* Numero de bits em um byte. */
  extern const long int kByteBits;

  /* Possiveis comandos. */
  extern const char *kCommands[];

  /* Diretorio root. */
  extern Directory kRoot;

  /* -- Functions -- */

  /* Tokeniza uma linha. */
  std::vector<std::string>& Tokenize(char *line);

  /* Mapeia um comando std::string -> int. */
  void (*CommandToFunction(const std::string& cmd)) (const std::vector<std::string>&);

  long int BytesToBlocks(long int bytes);

  namespace Time {
    time_t Get(void);
  }

  namespace BlockManager {
    /* FAT Table. */

#define SIZE_MEMORY_TABLE 25000
    /* Constant expression: kNumBlocks -> size. */
    extern Block* MemoryTable[SIZE_MEMORY_TABLE];

    namespace Bitmap {
      /* 0: free, 1: busy                */
      /* 1 char = 8 bits                 */
      /* k char = SIZE_MEMORY_TABLE bits */
      /* k = SIZE_MEMORY_TABLE/8 bits    */
      char* Map(void);
      long int Bits(void);
      bool Bit(long int index);
      void FlipBit(bool index);
      void SetBit(bool val, long int index);
    }
#undef SIZE_MEMORY_TABLE

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

        long int Next(void) { return next_; }
        long int Prev(void) { return prev_; }

        bool Ended() const { return index_<0 || index_>= kNumBlocks; }
        long int Index(void) const { return index_; }
      private:
        long int index_;
        long int next_;
        long int prev_;

        void Refresh(void) {
          if (index_ < 0)
            return;
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

    /* Comeco de uma FAT list. */
    Iterator Begin(Block *b);

    /* Final de uma FAT list. */
    Iterator End(Block *b);

    /* Numero de blocos livres. */
    long int Available(void);

    void SetBlock(long int index, Block *b);

    /* Encontra proximo bloco livre. */
    Block* NextAvailable(void);

    /* Encontra proximo bloco livre reversamente. */
    Block* NextAvailableRev(void);

    /* Libera o bloco do indice dado e todos seus subsequentes filhos. */
    void Free(long int i_block);

    /* Libera bloco e todos seus subsequentes filhos. */
    void Free(Block *b);
  }

  namespace Exception {
    class NoMemory : public std::exception {
      public:
        const char* message(void) const { return what(); }
      private:
        const char* what(void) const noexcept override {
          return "Exception NoMemory: no memory on FAT.";
        }
    };
  }
}

#endif /* _UTILS_HPP */
