//
// Created by Henry Little on 2019-06-08.
//

#ifndef MINISQL_B_PLUS_TREE_H
#define MINISQL_B_PLUS_TREE_H

#include <functional>
#include "../BufferManager/BufferManager.hpp"
#include "../RecordManager/RecordManager.hpp"
#include "../FileManager/FileManager.hpp"
#include "../BufferManager/Block.hpp"
#include "../utils/config.hpp"
#include "../Type/Value.hpp"


#define GETVAL(x) recordManager->getVal(columnName.c_str(), x)

#define NEXT_NODE(x) x->foffset[NODE_SIZE]
#define READ_NODE_BUF(x, y) x->setoffset(0); x->read(y, BLOCK_SIZE)
#define WRITE_BACK_NODE_BUF(x, y) x->setoffset(0); x->write(y, BLOCK_SIZE)
#define GET_IDX_BLOCK(x) bufferManager->getblock(MakeID(fileManager, x))
// a index key and a struct to find the record in the table file

const int NODE_SIZE = (BLOCK_SIZE - 2 * sizeof(short) - sizeof(unsigned long long)) /
                      (sizeof(unsigned long long) + sizeof(unsigned long long));
const int NODE_SIZE_HALF = NODE_SIZE / 2;
const int NODE_DELETED = 2;
struct BasicNode {
    short isLeaf;
    short size;
    unsigned long long roffset[NODE_SIZE]; // record value offset
    unsigned long long foffset[NODE_SIZE + 1]; // index file offset || record offset
};

// File manager returns blocknum
class BplusTree {
    using NodePT = struct BasicNode *;
    using BpRangeConsumer = std::function<void(int, unsigned long long)>;// consume a count and the file offset

    std::vector<BlockPtr> stack;
    std::vector<int> stackPos;
    int now = -1; //
    // External dependencies
    BlockPtr root = nullptr;
    FilePtr fileManager; // for the index file
    BufferPtr bufferManager;
    RecordPtr recordManager;
    std::string columnName;
public:
    BplusTree(
            const FilePtr &filePtr, const BufferPtr &buffer,
            const RecordPtr &record, BlockPtr root,
            std::string columnName, bool newTree);

    BlockPtr createNode() {
        int blockId = fileManager->allocblock();
        return bufferManager->getblock(MakeID(fileManager, blockId));
    }
    /* ---------- USED BY QUERY ------------ */
    /* query using Value for easy comparison */
    /* ------------------------------------- */
    BlockPtr findLeftMostNode();

    BlockPtr findNode(const Value &v);

    unsigned long long findOne(Value v);

    int findByRange(
            bool wl, bool leq, const Value &l,
            bool wr, bool req, const Value &r,
            const BpRangeConsumer &consumer
    );

    bool remove(const Value &v);

    void removeForm();

    // insert just record the offset to make it easy to maintain
    bool insert(unsigned long long roffset, unsigned long long foffset);

    void insertInto(unsigned long long roffset, const BlockPtr &nextBlock);

    int getRootNum() {
        return root->blocknumber();
    }
};

using BplusPtr = std::shared_ptr<BplusTree>;
#endif // MINISQL_B_PLUS_TREE_H