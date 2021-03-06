#include "utils.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <ctgmath>

#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#include "command.hpp"
#include "directory.hpp"
#include "block.hpp"
#include "stream.hpp"

namespace Utils {
  /* -- Constants -- */

  const char *kPrompt = "[ep3]: ";

  /* 100MB */
  const long int kSystemSize = 100000000;
  /* 4KB */
  const long int kBlockSize = 4000;

  const long int kNumBlocks = kSystemSize/kBlockSize;

  /* 2 bytes */
  const long int kPointerSize = 65536;
  const long int kPointerBytes = 2;

  const long int kByte = 256;
  const long int kByteBits = 8;

  const long int kFilesPerDir = 250;

  const char *kCommands[] = {"mount", "cp", "mkdir", "rmdir", "cat", "touch", "rm", "ls", "find",
    "df", "umount", "sai"};

  /* -- Functions -- */

  std::vector<std::string>& Tokenize(char *line) {
    static std::vector<std::string> a_table;

    a_table.clear();

    char *tok = strtok(line, " ");
    while (tok != NULL) {
      a_table.push_back(std::string(tok));
      tok = strtok(NULL, " ");
    }

    return a_table;
  }

  using fn_args = void (*) (const std::vector<std::string>&);

  fn_args CommandToFunction(const std::string& cmd) {
    static bool init = false;
    static std::map<std::string, fn_args> mapper;

    if (!init) {
      int s = sizeof(kCommands)/sizeof(*kCommands);
      for (int i = 0; i < s; ++i)
        mapper.insert(std::pair<std::string, fn_args>(kCommands[i], Command::kFunctions[i]));
      init = true;
    }

    fn_args val = nullptr;
    try {
      val = mapper.at(cmd);
    } catch(const std::out_of_range &exc) {
      fprintf(stderr, "%s, command not found\n", cmd.c_str());
      return nullptr;
    }

    return val;
  }

  long int BytesToBlocks(long int bytes) { return ceil(bytes/kBlockSize); }

  namespace Math {
    unsigned char FirstByte(long int n) {
      return n==0?0:((unsigned char) (n - (kByte * (kByte - 1))));
    }

    unsigned char SecondByte(long int n) {
      return n==0?0:((unsigned char)(n >> Utils::kByteBits));
    }
    /* Consider:
     * (A)_2 = a_1a_2a_3a_4 a_5a_6a_7a_8 = (buffer_[0])_10
     * (B)_2 = b_1b_2b_3b_4 b_5b_6b_7b_8 = (buffer_[1])_10
     * Then the sum of A + B is
     *  (A + B)_2 = a_1a_2a_3a_4 a_5a_6a_7a_8 b_1b_2b_3b_4 b_5b_6b_7b_8 = (next)_10 =
     *  ((A)_10 * 256) + (B)_10 = (next)_10
     */
    long int ComposeBytes(unsigned char a, unsigned char b) {
      return (long int)a * kByte + (long int)b;
    }
  }

  namespace Time {
    time_t Get(void) { return time(NULL); }
  }

  namespace BlockManager {
#define SIZE_MEMORY_TABLE 25000
    long int MemoryTable[SIZE_MEMORY_TABLE];

    namespace Bitmap {
#define CHAR_BITS_SIZE 8
      namespace {
        const long int kSizeChars = SIZE_MEMORY_TABLE/CHAR_BITS_SIZE;
        /* gcc guarantees zero intialization in compile time. */
        char bits_[kSizeChars] = {0};
      }
      char* Map(void) { return bits_; }

      long int Bits(void) { return SIZE_MEMORY_TABLE; }

      /* Takes the bit from index by shifting-right on the difference of the remainder.
       * e.g.: input  <- bit 23
       *       i_char = 23/8 = 2 (class 2, that is bits_[2] or second character that holds index)
       *       diff   = 23-2*8 = 23-16 = 7 (difference to the remainder of the position we want)
       *       shift diff-1 times byte[2]
       *         e.g.: let byte[2] = 10010100, we want bit 7 of byte[2]
       *               shift 10010100 >> 6 times = 10 (bit 8 and bit 7)
       *               we then get bit 7 by getting the remainder of the base (2), returning 0
       *       return remainder of 2 from previous operation's value.
       *
       * Note: gcc guarantees shift by 0 as identity shift.
       */
      /*bool Bit(long int index) {
        long int i_char = index/CHAR_BITS_SIZE;
        return (bits_[i_char]>>((index-i_char*CHAR_BITS_SIZE)-1))%2;
      }*/
      bool Bit(long int index) {
        return Stream::Input::ReadBit(index);
      }

      /* Flips the bit at index by XORing a constant multiple of 2 to flip. */
      /*void FlipBit(long int index) {
        long int i_char = index/CHAR_BITS_SIZE;
        long int diff = (index-i_char*CHAR_BITS_SIZE)-1;
        bits_[i_char]^=(1<<diff);
      }*/

      /* Does the same as FlipBit except if the value is the same, noop. */
      /*void SetBit(bool val, long int index) {
        long int i_char = index/CHAR_BITS_SIZE;
        long int diff = (index-i_char*CHAR_BITS_SIZE)-1;
        if ((bits_[i_char]>>diff)%2 != val)
          bits_[i_char]^=(1<<diff);
      }*/
      void SetBit(bool val, long int index) {
        Stream::Output::WriteBit(val, index);
      }
#undef CHAR_BITS_SIZE
    }

    const long int kEnd = -1;

    namespace {
      long int available_ = SIZE_MEMORY_TABLE;
    }

    Iterator::Iterator(long int index) : index_(index), next_(-1), prev_(-1) {
      Refresh();
    }
    Iterator::~Iterator(void) {}

    Iterator& Iterator::operator=(long int index) {
      index_ = index;
      Refresh();
      return *this;
    }

    Iterator& Iterator::operator=(const Iterator &it) {
      index_ = it.index_;
      Refresh();
      return *this;
    }

    Iterator& Iterator::operator++(void) {
      index_ = next_;
      Refresh();
      return *this;
    }

    Iterator& Iterator::operator--(void) {
      index_ = prev_;
      Refresh();
      return *this;
    }

    /*Iterator Begin(Block *b) {
      return b == nullptr? Iterator(0) : Iterator(b->Index());
    }

    Iterator End(Block *b) {
      if (b == nullptr)
        return Iterator(SIZE_MEMORY_TABLE-1);
      auto it = Begin(b);
      for (; it.Next() != kEnd; ++it);
      return it;
    }*/

    long int Available(void) { return available_; }

    void SetBlock(long int index, long int val) {
      MemoryTable[index] = val;
      /*Bitmap::SetBit(b==nullptr?false:true, index);*/
      Bitmap::SetBit(val, index);
    }

    long int NextAvailable(void) {
      for (long int i = Stream::Metadata::kDiskBlock.first; i < SIZE_MEMORY_TABLE; ++i)
        if (Bitmap::Bit(i))
          return i;
      return -1;
    }

    /*Block* NextAvailable(void) {
      for (long int i = 0; i < SIZE_MEMORY_TABLE; ++i)
        if (!Bitmap::Bit(i)) {
          Block *b = new Block(i, "");
          SetBlock(i, b);
          --available_;
          return b;
        }
      return nullptr;
    }*/

    /*Block* NextAvailableRev(void) {
      for (int i = SIZE_MEMORY_TABLE-1; i >= 0; --i)
        if (!Bitmap::Bit(i)) {
          Block *b = new Block(i, "");
          SetBlock(i, b);
          ++available_;
          return b;
        }
      return nullptr;
    }*/

    void Free(long int i_block) {
      if (i_block < 0 || !Bitmap::Bit(i_block))
        return;

      for (long int i = 0; i < SIZE_MEMORY_TABLE; i = MemoryTable[i]) {
        SetBlock(i, -1);
        --available_;
      }
    }

    /*void Free(Block *b) { Free(b->Index()); }*/
#undef SIZE_MEMORY_TABLE
  }
}
