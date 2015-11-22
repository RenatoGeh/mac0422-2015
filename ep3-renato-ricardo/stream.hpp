#ifndef _STREAM_HPP
#define _STREAM_HPP

#include <string>
#include <utility>
#include <exception>

#include "block.hpp"

namespace Stream {
  void Open(const std::string &filename);
  void Close(void);
  namespace Output {
    void WriteMeta(void);
    void WriteBit(bool val, long int index);
    void Write(long int index, const std::string &to_write);
    void WriteRegular(long int index, const std::string &content);
  }

  namespace Input {
    void ReadMeta(void);
    bool ReadBit(long int index);
    void ReadDirectory(long int index);
    /* Read one block. */
    std::string Read(long int index);
    /* Read all blocks starting from start and sets them to FAT. */
    std::string ReadRegular(long int start);
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
