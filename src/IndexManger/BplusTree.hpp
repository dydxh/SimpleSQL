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
#define NEXT_NODE(x) (x->foffset[NODE_SIZE])
#define READ_NODE_BUF(x, y) x->setoffset(0); x->read(y, NODE_SIZE)
#define GET_IDX_BLOCK(x) bufferManager->getblock(MakeID(fileManager, x))
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
    FilePtr fileManager; // for the index file
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
                blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->foffset[size]));
                READ_NODE_BUF(blk, nodeBuffer);
                continue;
            }
            for (int i; i < nodeBuffer->size; i++) {
                if (valcmp(GETVAL(nodeBuffer->roffset[i].i, v)) > 0) {
                    // val > v
                    stackPos.push_back(i);
                    blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->foffset[i]));
                    // update node buffer
                    READ_NODE_BUF(blk, nodeBuffer);
                    break;
                }
            }
            return blk;
        }
    }

    /* ---------- USED BY QUERY ------------ */
    long long findOne(Value v) {
        BlockPtr blk = findNode(v);
        NodePT nodeBuffer;
        blk->setoffset(0);
        blk->read(nodeBuffer, NODE_SIZE);
        for (int i = 0; i < nodeBuffer->size; i++) {
            if (0 == valcmp(GETVAL(nodeBuffer->roffset[i]), v)) {
                return nodeBuffer->roffset[i];
            }
        }
        return 0;
    }

    BlockPtr findLeftMostNode() {
        BlockPtr blk = root;
        NodePT nodeBuffer;
        READ_NODE_BUF(blk, nodeBuffer);
        while (!nodeBuffer->isLeaf) {
            blk = GET_IDX_BLOCK(nodeBuffer->foffset[0]);
            READ_NODE_BUF(blk, nodeBuffer);
        }
        return blk;
    }

    // withLeft? leftEqual? leftValue
    int findByRange(
            bool wl, bool leq, Value l,
            bool wr, bool req, Value r,
            BpRangeConsumer consumer
    ) {
        int count = 0;
        Block blk;
        NodePT nodeBuffer;
        int now = 0;
        if (wl) {
            blk = findNode(l);
            READ_NODE_BUF(blk, nodeBuffer);
            bool find = false;
            // find? the lower bound
            for (int i = o; i < nodeBuffer->size; i++) {
                if (valcmp(GETVAL(nodeBuffer->roffset[i]), l) > 0
                    || (valcmp(GETVAL(nodeBuffer->roffset[i]), l) == 0 && leq)) {
                    // val > l
                    find = true;
                    now = i;
                    break;
                }
            }
            if (!find) {
                if (0 == NEXT_NODE(nodeBuffer)) return 0;
                blk = GET_IDX_BLOCK(NEXT_NODE(nodeBuffer)));
                READ_NODE_BUF(blk, nodeBuffer);
            }
        } else { // !wl
            blk = findLeftMostNode();
            READ_NODE_BUF(blk, nodeBuffer);
        }
        while (true) {
            for (int i = now; i < nodeBuffer->size; i++) {
                if (wr && (valcmp(GETVAL(nodeBuffer->roffset[i]), r) > 0
                           || (valcmp(GETVAL(nodeBuffer->roffset[i]), r) == 0 && !req))) {
                    return count;
                }
                consumer(count++, nodeBuffer->foffset[i]);
            }
        }
    }
    // IMPORTANT used to insert new value to B+ Tree
    bool insert(Value v, long long foffset) {
        BlockPtr blk = findNode(v);
        NodePT nodeBuffer;
        READ_NODE_BUF(blk, nodeBuffer);
        if (nodeBuffer->size < NODE_SIZE) {
            // the node is not full, just insert

        }
    }
};

using BpRangeConsumer = std::function<void(int, long long)>; // consume a count and the file offset

#endif // MINISQL_B_PLUS_TREE_H