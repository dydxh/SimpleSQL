//
// Created by Henry Little on 2019-06-08.
//

#ifndef MINISQL_B_PLUS_TREE_H
#define MINISQL_B_PLUS_TREE_H

#include "../utils/config.hpp"
#include "../FileManager/FileManager.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../BufferManager/Block.hpp"
#include "utils/TypeUtils.hpp"
// a index key and a struct to find the record in the table file

struct RecordIndex {
    int blk;
    int offset;
};

// need to compute the node size dynamically
template<typename T>
class NodeSize {
public:
    static const int NODE_SIZE;
};

// stores exact one node in one block
template<typename T>
const int NodeSize<T>::NODE_SIZE =
        (BLOCK_SIZE - 2 * sizeof(short) - sizeof(int)) / (sizeof(T) + sizeof(int));

#define HALF_NODE_SIZE = ((NODE_SIZE + 1) / 2)
template<typename T>
struct BaseNode {
    short isLeaf;
    short size;
    T key[NodeSize<T>::NODE_SIZE];
    // one more value than keys
    int blknum[NodeSize<T>::NODE_SIZE + 1];
};

template<typename T>
const int BplusTree<T>::NODE_SIZE = NodeSize<T>::NODE_SIZE;

// File manager returns blocknum
template<typename T>
class BplusTree {
    static const int NODE_SIZE;
    using Node = BaseNode<T>;

    int now = -1;

    BlockPtr root = 0;
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
    void insertInto(T v, BlockPtr nextBlock) {
        if (now == -1) {
            // root
            BlockPtr block = createNode();
            void *src;
            nextBlock->setoffset(0);
            nextBlock->read(src, BLOCK_SIZE);
            block->write(src, BLOCK_SIZE); // automatic dirty
            // update root node
            Node node = {.isLeaf = false, .size = 1};
            node.blknum[0] = block->blocknumber();
            node.blknum[1] = nextBlock->blocknumber();
            TypeUtil<T>::set(node.key[0], v);
            root->setoffset(0);
            root->write(&node, BLOCK_SIZE);
        } else {
            BlockPtr block = stack[now];
            block->setoffset(sizeof(short));
            short size;
            block->read(&size, sizeof(short));
            int pos = stackPos[now];
            if (size < NODE_SIZE) {
                // shift right by 1 to insert
                for (int i = size; i > pos; i--) {
                    block->setoffset(2 * sizeof(short) + i * sizeof(T))
                    TypeUtil<T>::set(node->key[i], node->key[i - 1]);
                    node->blknum[i + 1] = node->blknum[i];
                }
                // do the insertion
                TypeUtil<T>::set(node->key[pos], v);
                node->blknum[pos + 1] = nextBlock->blocknumber();
                node->size ++;

            } else {

            }
        }
    }
};


#endif // MINISQL_B_PLUS_TREE_H