#ifndef minisql_block_header
#define minisql_block_header

#include <cstdio>
#include <cstring>
#include <string>
#include <memory>
#include <utility>
#include "../utils/config.hpp"
#include "../FileManager/FileManager.hpp"

using BlockID = std::pair<FilePtr, int>;

extern BlockID MakeID(FilePtr file, const int blockid);

class Block {
public:
    Block(const BlockID& bid);
    void read(void* dst, unsigned int size);
    void write(void* src, unsigned int size);
    void flush();

    inline int fileidx() {return file->fileidx();}
    inline FilePtr fileptr() {return file;}
    inline int blocknumber() {return blocknum;}
    inline bool isfree() {return free;}
    inline void setfree(bool val) {free = val;}
    inline bool isdirty() {return dirty;}
    inline void setdirty(bool val) {dirty = val;}
    inline bool ispinned() {return pinned;}
    inline void setpinned(bool val) {pinned = val;}
    inline void setoffset(int newoffset) {offset = newoffset;}

private:
    FilePtr file;
    int blocknum;
    bool free, dirty, pinned;
    int offset;
    char buffer[BLOCK_SIZE];
};

using BlockPtr = std::shared_ptr<Block>;

#endif