#ifndef minisql_buffermanager_header
#define minisql_buffermanager_header

#include <memory>
#include <cstdio>
#include <list>

#include "Block.hpp"
#include "../FileManager/FileManager.hpp"

class BufferManager {
public:
    BufferManager();
    ~BufferManager();

    static void allocBlock(FilePtr file);

    BlockPtr getblock(const BlockID& blockid);
    void cleancache();

    void flush();

private:
    std::list<BlockPtr> cache;
};
using BufferPtr = std::shared_ptr<BufferManager>;
#endif