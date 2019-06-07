#include "Block.hpp"
#include "../utils/ErrorManager.hpp"

Block::Block(const BlockID& bid) {
    file = bid.first;
    blocknum = bid.second;
    free = true;
    dirty = pinned = false;
    offset = 0;
    fread(buffer, BLOCK_SIZE, 1, file->handler());
}

void Block::read(void* dst, unsigned int size) {
    if(offset + size > BLOCK_SIZE)
        throw OutofBoundary("[Error] Try to read data out of block boundary.");
    memcpy(dst, buffer + offset, size);
    offset += size;
}

void Block::write(void* src, unsigned int size) {
    if(offset + size > BLOCK_SIZE)
        throw OutofBoundary("[Error] Try to write data out of block boundary.");
    memcpy(buffer + offset, src, size);
    offset += size;
    dirty = true;
}

void Block::flush() {
    file->writeblock(this->blocknum, this->buffer);
}
