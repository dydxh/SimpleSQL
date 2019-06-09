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

#define GETVAL(x) x
// a index key and a struct to find the record in the table file

const int NODE_SIZE = (BLOCK_SIZE - 2 * sizeof(short) - sizeof(long long)) / (sizeof(long long) + sizeof(long long));


struct BasicNode {
    short isLeaf;
    short size;
    long long roffset[NODE_SIZE]; // record offset
    long long foffset[NODE_SIZE + 1];
};

// File manager returns blocknum
class BplusTree {
    using NodePT = struct BasicNode *;

    BlockPtr root = 0;

    int now = -1;
    std::vector<BlockPtr> stack;
    std::vector<int> stackPos;
    // External dependencies
    // RecordPtr recordManager
    FilePtr fileManager;
    BufferPtr bufferManager;

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
        // BlockPtr block = 
    }

    BlockPtr findNode(Value v) {
        stack.clear();
        stackPos.clear();
        // read all the value to a temp
        NodePT nodeBuffer;
        BlockPtr blk = root;
        blk->setoffset(0);
        blk->read(nodeBuffer, NODE_SIZE);
        while (!nodeBuffer->isLeaf) {
            stack.push_back(blk);
            // TODO: implement GETVAL
            if (valcmp(GETVAL(nodeBuffer->roffset[nodeBuffer->size - 1]), v) <= 0) {
                // val <= v
                stackPos.push_back(nodeBuffer->size);
                blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->roffset[size]));
                blk->setoffset(0);
                blk->read(nodeBuffer, NODE_SIZE);
                continue;
            }
            for (int i; i < nodeBuffer->size; i++) {
                if (valcmp(GETVAL(nodeBuffer->roffset[i].i, v)) > 0) {
                    // val > v
                    stackPos.push_back(i);
                    blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->roffset[i]));
                    // update node buffer
                    blk->setoffset(0);
                    blk->read(nodeBuffer, NODE_SIZE);
                    break;
                }
            }
            return blk;
        }
    }

    /* ---------- USED BY QUERY ------------ */
    long long findOne(Value v) {
        BlockPtr blk = findNode(v);
    }
};


#endif // MINISQL_B_PLUS_TREE_H