#include "BufferManager.hpp"
#include "../utils/config.hpp"

void BufferManager::allocBlock(FilePtr file) {
    file->allocblock();
}

BufferManager::BufferManager() {}

BufferManager::~BufferManager() {
    for (auto iter = cache.begin(); iter != cache.end(); iter++) {
        BlockPtr ptr = *iter;
        if (ptr->isdirty())
            ptr->flush();
    }
    cache.clear();
}

BlockPtr BufferManager::getblock(const BlockID &blockid) {
    for (auto iter = cache.begin(); iter != cache.end(); iter++) {
        if (blockid.first == (*iter)->fileptr() &&
            blockid.second == (*iter)->blocknumber()) {
            cache.push_front((*iter));
            cache.erase(iter);
            return *iter;
        }
    }
    cleancache();
    BlockPtr retblock = std::make_shared<Block>(blockid);
    cache.push_back(retblock);
    return retblock;
}

void BufferManager::cleancache() {
    int overflow = cache.size() - MAX_CACHE_NUMBER + 1;
    for (auto iter = cache.rbegin(); overflow > 0 && iter != cache.rend(); iter++) {
        if ((*iter)->ispinned()) continue;
        else if ((*iter)->isdirty()) {
            (*iter)->flush();
            (*iter)->setdirty(false);
        }
        cache.erase(std::next(iter).base());
        overflow -= 1;
    }
}

void BufferManager::flush() {
    for (auto iter = cache.begin(); iter != cache.end(); iter++) {
        if ((*iter)->isdirty()) {
            (*iter)->flush();
            (*iter)->setdirty(false);
        }
    }
}
