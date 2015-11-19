#include <cstdio>
#include <climits>
#include <cstring>
#include <unistd.h>

#include "stream.hpp"
#include "utils.hpp"
#include "block.hpp"

namespace Stream {
#define BLOCK_SIZE 4000
  namespace {
    using uchar = unsigned char;
    uchar buffer_[BLOCK_SIZE];

    FILE *in_stream_ = NULL;
    FILE *out_stream_ = NULL;

    bool Exists(const std::string &filename) { return access(filename.c_str(), F_OK) != -1; }
  }

  namespace Output {
    void Open(const std::string &filename) {
      bool exists = Exists(filename);
      out_stream_ = fopen(filename.c_str(), "wb");
      if (out_stream_ == NULL)
        throw Exception::InvalidFile();
      if (!exists) {
        /* Set all content to 0. */
        memset(buffer_, 0, BLOCK_SIZE);
        for (long int i = 0; i < Utils::kNumBlocks; ++i)
          fwrite(buffer_, sizeof(uchar), sizeof(buffer_), out_stream_);
      }
    }

    void Write(Block *head) {
      if (out_stream_ == NULL)
        throw Exception::InvalidFile();
      long int index = head->Index();
      uchar data[BLOCK_SIZE];
      Block *c_block = head;

      for (auto it = Utils::BlockManager::Begin(head); !it.Ended(); ++it) {
        /* TODO: Explain on report. */
        data[1] = (uchar)(index - (Utils::kByte * (Utils::kByte - 1)));
        data[0] = (uchar)(index >> Utils::kByteBits);
        const char *content = c_block->Read().c_str();

        long int s_content = c_block->Read().length();
        long int offset = Utils::kPointerBytes;
        for (long int i = offset; i < s_content ||  i < BLOCK_SIZE; ++i)
          data[i] = (uchar) content[i-offset];

        fseek(out_stream_, index*Utils::kNumBlocks, SEEK_SET);
        fwrite(data, sizeof(uchar), sizeof(uchar)*BLOCK_SIZE, out_stream_);
        c_block = *it;
        index = c_block->Index();
      }
    }

    void Close(void) { fclose(out_stream_); }
  }

  namespace Input {
    void Open(const std::string &filename) {
      in_stream_ = fopen(filename.c_str(), "rb");
      if (in_stream_ == NULL)
        throw Exception::InvalidFile();
    }

    Block* Read(long int index) {
      if (out_stream_ == NULL)
        throw Exception::InvalidFile();
      fseek(in_stream_, index*Utils::kNumBlocks, SEEK_SET);
      fread(buffer_, sizeof(uchar), sizeof(buffer_), in_stream_);

      /* Consider:
       * (A)_2 = a_1a_2a_3a_4 a_5a_6a_7a_8 = (buffer_[0])_10
       * (B)_2 = b_1b_2b_3b_4 b_5b_6b_7b_8 = (buffer_[1])_10
       * Then the sum of A + B is
       *  (A + B)_2 = a_1a_2a_3a_4 a_5a_6a_7a_8 b_1b_2b_3b_4 b_5b_6b_7b_8 = (next)_10 =
       *  ((A)_10 * 256) + (B)_10 = (next)_10
       */
      long int next = (long int)buffer_[1] + (long int)(buffer_[0] * Utils::kByte);
      char converter[BLOCK_SIZE];

      long int offset = Utils::kPointerBytes;
      for (long int i = offset; i < BLOCK_SIZE; ++i)
        converter[i-offset] = (char) buffer_[i];

      Block *b = new Block(index, std::string(converter));
      b->SetNext(next);
      return b;
    }

    void ReadAll(long int start) {
      Block *b = Read(start);
      long int next = b->Next();

      while (next != Utils::BlockManager::kEnd) {
        long int prev = b->Index();
        Utils::BlockManager::MemoryTable[prev] = b;
        b = Read(next);
        b->SetPrev(prev);
        next = b->Next();
      }
    }

    void Close(void) { fclose(in_stream_); }
  }
#undef BLOCK_SIZE
}
