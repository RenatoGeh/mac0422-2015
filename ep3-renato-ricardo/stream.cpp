#include <cstdio>
#include <climits>
#include <cstring>
#include <unistd.h>
#include <utility>

#include "stream.hpp"
#include "utils.hpp"
#include "block.hpp"

namespace Stream {
#define BLOCK_SIZE 4000
  namespace {
    using uchar = unsigned char;
    uchar buffer_[BLOCK_SIZE];

    FILE *stream_ = NULL;

    bool Exists(const std::string &filename) { return access(filename.c_str(), F_OK) != -1; }
  }

  void Open(const std::string &filename) {
    bool exists = Exists(filename);
    stream_ = fopen(filename.c_str(), "rb+");
    if (stream_ == NULL)
      throw Exception::InvalidFile();
    if (!exists) {
      /* Set all content to 0. */
      memset(buffer_, 0, BLOCK_SIZE);
      for (long int i = 0; i < Utils::kNumBlocks; ++i)
        fwrite(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
    }
  }

  void Close(void) { fclose(stream_); }

  namespace Output {
    namespace {
#define SIZE_MEMORY_TABLE 25000
#define CHAR_BITS_SIZE 8
      void WriteBitmap(void) {
        /* Write bitmap. */
        char *bitmap = Utils::BlockManager::Bitmap::Map();
        long int s_bitmap = Utils::kNumBlocks/Utils::kByteBits;

        uchar data[SIZE_MEMORY_TABLE/CHAR_BITS_SIZE];

        for (long int i = 0; i < s_bitmap; ++i)
          data[i] = (uchar) bitmap[i];

        fseek(stream_, Metadata::kBitmapBlock.first*Utils::kBlockSize, SEEK_SET);
        fwrite(data, sizeof(uchar), sizeof(data), stream_);
      }

      void WriteFat(void) {
        uchar data[2*SIZE_MEMORY_TABLE] = {0};
        long int nb_size = 2 * Utils::kNumBlocks;
        for (long int i = 0; i < nb_size; i+=2) {
          long int b_val = Utils::BlockManager::Bitmap::Bit(i/2)?
            Utils::BlockManager::MemoryTable[i] : 0;
          data[i] = Utils::Math::SecondByte(b_val);
          data[i+1] = Utils::Math::FirstByte(b_val);
        }

        fseek(stream_, Metadata::kFatBlock.first*Utils::kBlockSize, SEEK_SET);
        fwrite(data, sizeof(uchar), sizeof(data), stream_);
      }

      void WriteRoot(void) {
      }
    }

    void WriteMeta(void) {
      if (stream_ == NULL)
        throw Exception::InvalidFile();

      WriteBitmap();
      WriteFat();
      WriteRoot();
    }

    void WriteBit(bool val, long int index) {
      long int i_char = index/CHAR_BITS_SIZE;
      long int diff = (index-i_char*CHAR_BITS_SIZE)-1;
      fseek(stream_, Metadata::kBitmapBlock.first*Utils::kBlockSize+i_char, SEEK_SET);
      uchar byte;
      fread(&byte, sizeof(uchar), sizeof(uchar), stream_);
      if ((byte>>diff)%2 != val)
        byte^=(1<<diff);
      fseek(stream_, Metadata::kBitmapBlock.first*Utils::kBlockSize+i_char, SEEK_SET);
      fwrite(&byte, sizeof(uchar), sizeof(byte), stream_);
    }

    void Write(long int index, const std::string &to_write) {
      if (stream_ == NULL)
        throw Exception::InvalidFile();
      uchar data[BLOCK_SIZE];

      /* TODO: Explain on report. */
      data[1] = Utils::Math::FirstByte(index);
      data[0] = Utils::Math::SecondByte(index);
      const char *content = to_write.c_str();
      long int s_content = to_write.length();
      long int offset = Utils::kPointerBytes;
      for (long int i = offset; i < s_content ||  i < BLOCK_SIZE; ++i)
        data[i] = (uchar) content[i-offset];

      fseek(stream_, index*Utils::kBlockSize, SEEK_SET);
      fwrite(data, sizeof(uchar), sizeof(uchar)*BLOCK_SIZE, stream_);
    }

    void WriteRegular(long int index, const std::string &content) {
      fseek(stream_, index*Utils::kBlockSize, SEEK_SET);
      long int n_ptr = Utils::BlockManager::MemoryTable[index];
      if (!Utils::BlockManager::Bitmap::Bit(index)) {
        fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
        Utils::BlockManager::SetBlock((n_ptr = Utils::Math::ComposeBytes(buffer_[1], buffer_[0])),
            index);
      }
      int n_blocks = Utils::BytesToBlocks(4 + content.length() +
          2*content.length()/Utils::kBlockSize);
      std::string partition = content.substr(0, Utils::kBlockSize-6);
      for (int i=0;i<Utils::kBlockSize-6;++i)
        buffer_[i] = (uchar) partition.at(i);
      for (int i=0;i<n_blocks;++i) {
        long int new_block = Utils::BlockManager::NextAvailable();
        partition = content.substr(Utils::kBlockSize-6 + i*(Utils::kBlockSize-2),
            Utils::kBlockSize-2);
        if (new_block < 0)
          throw Utils::Exception::NoMemory();
        for (int j=0;j<Utils::kBlockSize-2;++j)
          buffer_[j+2] = (uchar) partition.at(j);
        fseek(stream_, new_block*Utils::kBlockSize, SEEK_SET);
        fwrite(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
      }
    }

#undef CHAR_BITS_SIZE
#undef SIZE_MEMORY_TABLE
  }

  namespace Input {
    namespace {
      void ReadFat() {
        fseek(stream_, Metadata::kFatBlock.first*Utils::kBlockSize, SEEK_SET);
        fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
        int i;
        for (i = 0; i < Utils::kNumBlocks; i+=2)
          Utils::BlockManager::MemoryTable[i]=Utils::Math::ComposeBytes(buffer_[i], buffer_[i+1]);
        fseek(stream_, (Metadata::kFatBlock.first+1)*Utils::kBlockSize, SEEK_SET);
        fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
        for (;i < 2*Utils::kNumBlocks; i+=2)
          Utils::BlockManager::MemoryTable[i]=Utils::Math::ComposeBytes(buffer_[i], buffer_[i+1]);
      }

      void ReadRoot() {
        ReadDirectory(Metadata::kRootBlock.first);
      }
    }

    void ReadDirectory(long int index) {
      long int num_files, pointer;
      bool is_direc;
      fseek(stream_, index*Utils::kBlockSize, SEEK_SET);
      fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);
      /*Pega o bit mais significatio, que diz se é um diretório ou não*/
      is_direc = (buffer_[2]>>7)%2;
      pointer  = Utils::Math::ComposeBytes(buffer_[1], buffer_[0]);
      Utils::BlockManager::SetBlock(index, pointer);
      if (is_direc) {
        /*Caso seja um diretório*/
        num_files = (long int) buffer_[3];

        /*Começa iteração por todos arquivos do diretório*/
        for (long int i = 0; i< num_files; i++) {
          pointer = Utils::Math::ComposeBytes(buffer_[4+i*31], buffer_[5+i*31]);
          ReadDirectory(pointer);
        }
      }
    }

    void ReadMeta(void) {
      if (stream_ == NULL)
        throw Exception::InvalidFile();
      ReadFat();
      ReadRoot();
    }

#define CHAR_BITS_SIZE 8
    bool ReadBit(long int index) {
      long int i_char = index/CHAR_BITS_SIZE;
      fseek(stream_, Metadata::kBitmapBlock.first*Utils::kBlockSize+i_char, SEEK_SET);
      uchar byte;
      fread(&byte, sizeof(uchar), sizeof(uchar), stream_);
      return (byte>>((index-i_char*CHAR_BITS_SIZE)-1))%2;
    }
#undef CHAR_BITS_SIZE

    std::string Read(long int index) {
      if (stream_ == NULL)
        throw Exception::InvalidFile();
      fseek(stream_, index*Utils::kBlockSize, SEEK_SET);
      fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);

      char converter[BLOCK_SIZE];

      long int offset = Utils::kPointerBytes;
      for (long int i = offset; i < BLOCK_SIZE; ++i)
        converter[i-offset] = (char) buffer_[i];

      return std::string(converter);
    }

    std::string ReadRegular(long int start) {
      int i = start;
      fseek(stream_, i*Utils::kBlockSize, SEEK_SET);
      fread(buffer_, sizeof(uchar), sizeof(buffer_), stream_);

      std::string text;
      for (int i=4;i<Utils::kBlockSize;++i)
        text.push_back((char)buffer_[i]);

      while (i > 0) {
        text.append(Read(i));
        i = Utils::Math::ComposeBytes(buffer_[1], buffer_[0]);
      }

      return text;
    }

  }

  namespace Metadata {
    const std::pair<long int, long int> kBitmapBlock(
        0, Utils::BytesToBlocks(Utils::kNumBlocks/Utils::kByteBits));
    const std::pair<long int, long int> kFatBlock(
        kBitmapBlock.first + kBitmapBlock.second,
        Utils::BytesToBlocks(Utils::kPointerBytes*(Utils::kSystemSize/Utils::kBlockSize)));
    const std::pair<long int, long int> kRootBlock(
        kFatBlock.first + kFatBlock.second, 2);
    const std::pair<long int, long int> kDiskBlock(
        kRootBlock.first + kRootBlock.second, Utils::kNumBlocks-kRootBlock.first);
  }
#undef BLOCK_SIZE
}
