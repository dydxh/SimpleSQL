//
// Created by Henry Little on 2019-06-08.
//

#ifndef MINISQL_B_PLUS_TREE_H
#define MINISQL_B_PLUS_TREE_H

#include "../utils/config.hpp"
#include "../FileManager/FileManager.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../BufferManager/Block.hpp"
#include "../Type/Value.hpp"
// a index key and a struct to find the record in the table file

const int NODE_SIZE = (BLOCK_SIZE - 2 * sizeof(short) - sizeof(int)) / (sizeof(Value) + sizeof(int));



struct BasicNode {
    short isLeaf;
    short size;
    long long roffset[NODE_SIZE]; // record offset
    long long foffset[NODE_SIZE + 1]; // per file offset 
};

// File manager returns blocknum
class BplusTree {
    static const int NODE_SIZE;
    using NodePtr = struct BasicNode *;

    int now = -1;
    
    BlockPtr root = 0;
    // Externel dependencies
    FilePtr fileManager;
    BufferPtr bufferManager;

    std::vector<BlockPtr> stack;
    std::vector<int> stackPos;
public:
    // TODO: Constructor
    BlockPtr createNode() {
        int blockId = fileManager->allocblock();
        return bufferManager->getblock(MakeID(fileManager, blockId));
    }

    /*
     *  Insert the overflow index node into another node
     * */
    void insertInto(Value v, BlockPtr nextBlock) {

    }

    bool insert(Value key, int value) {
        BlockPtr block = 
    }

    BlockPtr findNode(Value v) {
        stack.clear();
        stackPos.clear();
        BlockPtr blk = root;
        short isLeaf;
        blk->setoffset(0);
        blk->read(isLeaf, sizeof(short));
        while (!isLeaf) {
            stack.push_back(blk);
            Value val;
            int size;
            blk->setoffset(2);
            blk->read(&size, sizeof(short));
            blk->setoffset(4 + sizeof(Value) * (size - 1));
            blk->read(&val, sizeof(Value));
            if (valcmp(val, v) <= 0) {
                // val <= v
                stackPos.push_back(size);
                blk->setoffset(4 + sizeof(Value) * (NODE_SIZE - 1) + sizeof(int) * size);
                int blknum;
                blk->read(&blknum, sizeof(int));
                blk = bufferManager->getblock(MakeID(fileManager, blknum));
                continue;
            }
            for (int i = 0; i < size; i++) {
                blk->setoffset(4 + sizeof(Value) * i);
                blk->read(&val, sizeof(Value));
                if (valcmp(val, v) > 0) {
                    stackPos.push_back(i);
                    blk->setoffset(4 + sizeof(Value) * (NODE_SIZE - 1) + sizeof(int) * i);
                    int blknum;
                    blk->read(&blknum, sizeof(int));
                    blk = bufferManager->getblock(MakeID(fileManager, blknum));
                    break;
                }
            }
            blk->setoffset(0);
            blk->read(&isLeaf, sizeof(short));
        }
        return blk;
    }
};


#endif // MINISQL_B_PLUS_TREE_H