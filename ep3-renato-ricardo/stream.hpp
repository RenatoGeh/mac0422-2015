#ifndef _STREAM_HPP
#define _STREAM_HPP

#include <string>
#include <exception>

#include "block.hpp"

namespace Stream {
  namespace Output {
    void Open(const std::string &filename);
    void WriteMeta(void);
    void Write(Block *head);
    void Close(void);
  }

  namespace Input {
    void Open(const std::string &filename);
    void ReadMeta(void);
    /* Read one block. */
    Block* Read(long int index);
    /* Read all blocks starting from start and sets them to FAT. */
    void ReadAll(long int start);
    void ReadDirectory(long int index);
    void Close(void);
  }

  namespace Metadata {
    /* Pairs -> (pos, length). */
    extern const std::pair<long int, long int> kBitmapBlock;
    extern const std::pair<long int, long int> kFatBlock;
    extern const std::pair<long int, long int> kRootBlock;
    extern const std::pair<long int, long int> kDiskBlock;
  }

  namespace Exception {
    class InvalidFile : public std::exception {
      public:
        const char* message(void) const { return what(); }
      private:
        const char* what(void) const noexcept override {
          return "Exception InvalidFile: file could not be opened.";
        }
    };
  }
}

#endif
