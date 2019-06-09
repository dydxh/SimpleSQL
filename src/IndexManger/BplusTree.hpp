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
//TODO: Implement GETVAL & SETVAL form record manager
// TODO: GETVAL is equivalent to <read from the record file with the given record offset>
// TODO: SETVAL is equivalent to <get the offset from a Value{maybe can be changed to offset directly}>
#define GETVAL(x) x
#define SETVAL(x, v) x

#define NEXT_NODE(x) x->foffset[NODE_SIZE]
#define READ_NODE_BUF(x, y) x->setoffset(0); x->read(y, NODE_SIZE)
#define WRITE_BACK_NODE_BUF(x, y) x-setoffset(0); x->write(y, NODE_SIZE)
#define GET_IDX_BLOCK(x) bufferManager->getblock(MakeID(fileManager, x))
// a index key and a struct to find the record in the table file

const int NODE_SIZE = (BLOCK_SIZE - 2 * sizeof(short) - sizeof(long long)) / (sizeof(long long) + sizeof(long long));
const int NODE_SIZE_HALF = NODE_SIZE / 2;

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
    // RecordPtr recordManager; // contains a bufferManager to the record file
    FilePtr fileManager; // for the index file
    BufferPtr bufferManager;

public:
    // TODO: Constructor
    BlockPtr createNode() {
        int blockId = fileManager->allocblock();
        return bufferManager->getblock(MakeID(fileManager, blockId));
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
    bool insert(long long roffset, long long foffset) {
        BlockPtr blk = findNode(roffset);
        NodePT nodeBuffer;
        READ_NODE_BUF(blk, nodeBuffer);
        if (nodeBuffer->size < NODE_SIZE) {
            // the node is not full, just insert
            for (int i = nodeBuffer->size; i >= 1; i--) {
                if (valcmp(GETVAL(nodeBuffer->roffset[i - 1]), roffset) < 0) {
                    // val < v
                    SETVAL(nodeBuffer->roffset[i], roffset);
                    nodeBuffer->foffset[i] = foffset;
                    x->size++;
                    // write back to block
                    WRITE_BACK_NODE_BUF(blk, nodeBuffer);
                    return true; // insertion complete
                } else if (0 == valcmp(GETVAL(nodeBuffer->roffset[i - 1]), roffset)) {
                    return false; // duplicate value
                } else {
                    nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                    nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
                }
            }
            SETVAL(nodeBuffer->roffset[0]);
            nodeBuffer->foffset[0] = foffset;
            x->size++;
            // write back to block
            WRITE_BACK_NODE_BUF(blk, nodeBuffer);
        } else {
            // if there is a equal key
            for (int i = nodeBuffer->size; i >= 1; i--) {
                if (0 == valcmp(GETVAL(nodeBuffer->roffset[i - 1]), roffset)) return false;
            }
            // split the node into [HALF_NODE_SIZE, HALF_NODE_SIZE]
            // then do a recursive insertion
            BlockPtr nblk = createNode();
            NodePT nNodeBuffer;
            READ_NODE_BUF(nblk, nNodeBuffer);
            NEXT_NODE(nNodeBuffer) = NEXT_NODE(nodeBuffer);
            NEXT_NODE(nodeBuffer) = blk->blocknumber();
            nNodeBuffer->isLeaf = true;
            nNodeBuffer->size = 0;
            nodeBuffer->size = NODE_SIZE_HALF;
            if (valcmp(GETVAL(nodeBuffer->roffset[NODE_SIZE_HALF - 1]), roffset) < 0) {
                // val < v
                // nodeBuffer do not requires further modifications -> write back to buffer
                bool flag = false; // weather the value is inserted
                for (int i = NODE_SIZE_HALF; i < NODE_SIZE; i++) {
                    if (!flag && valcmp(GETVAL(nodeBuffer->roffset[NODE_SIZE_HALF - 1]), roffset) < 0) {
                        // val < v
                        SETVAL(nNodeBuffer->roffset[nNodeBuffer->size], roffset);
                        nNodeBuffer->foffset[nNodeBuffer->size++] = foffset;
                        flag = true;
                    }
                    nNodeBuffer->roffset[nNodeBuffer->size] = nodeBuffer->roffset[i];
                    nNodeBuffer->foffset[nNodeBuffer->size++] = nodeBuffer->foffset[i];
                }
                if (!flag) {
                    SETVAL(nNodeBuffer->roffset[nNodeBuffer->size], roffset);
                    nNodeBuffer->foffset[nNodeBuffer->size++] = foffset;
                }
            } else {
                bool flag = false;
                // new node needs only copy
                for (int i = NODE_SIZE_HALF - 1; i < NODE_SIZE; i++) {
                    nNodeBuffer->roffset[nNodeBuffer->size] = nodeBuffer->roffset[i];
                    nNodeBuffer->foffset[nodeBuffer->size++] = nodeBuffer->foffset[i];
                }
                // origin node needs a insertion
                for (int i = NODE_SIZE_HALF - 1; i >= 1; i--) {
                    if (valcmp(GETVAL(nodeBuffer->roffset[i - 1]), roffset) < 0) {
                        // val < v
                        SETVAL(nodeBuffer->roffset[i], roffset);
                        nodeBuffer->foffset[i] = foffset;
                        flag = true;
                        break;
                    } else {
                        nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                        nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
                    }
                }
                if (!flag) {
                    SETVAL(nodeBuffer->roffset[0], roffset);
                    nodeBuffer->foffset[0] = foffset;
                }
            }
            // remember to write back nodeBuffer to blk
            WRITE_BACK_NODE_BUF(blk, nodeBuffer);
            WRITE_BACK_NODE_BUF(nblk, nNodeBuffer);
            stack.push_back(blk);
            insertInto(GETVAL(nNodeBuffer->roffset[0]), nblk);// update the B+ tree from bottom up
        }
        // sure the data in write back to the buffer
        WRITE_BACK_NODE_BUF(blk, nodeBuffer);
        WRITE_BACK_NODE_BUF(nblk, nNodeBuffer);
        return true;
    }

    /*
     *  Insert the overflow node into the above node (a recursive update process)
     */
    void insertInto(long long v, BlockPtr nextBlock) {

    }
};

using BpRangeConsumer = std::function<void(int, long long)>; // consume a count and the file offset

#endif // MINISQL_B_PLUS_TREE_H