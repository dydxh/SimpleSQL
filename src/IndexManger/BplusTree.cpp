//
// Created by Henry Little on 2019-06-08.
//
#include "BplusTree.hpp"


BplusTree::BplusTree(const FilePtr &filePtr, const BufferPtr &buffer, const RecordPtr &record, const BlockPtr root,
                     const std::string columnName) {
    this->fileManager = filePtr;
    this->bufferManager = bufferManager;
    this->recordManager = record;
    this->root = root;
    this->columnName = columnName;
    // Initialize root node
    NodePT nodeBuffer = new BasicNode;
    READ_NODE_BUF(root, nodeBuffer);
    NEXT_NODE(nodeBuffer) = 0;
    nodeBuffer->isLeaf = true;
    nodeBuffer->size = 0;
    WRITE_BACK_NODE_BUF(root, nodeBuffer);
}

BlockPtr BplusTree::findNode(const Value &v) {
    stack.clear();
    stackPos.clear();
    // read all the value to a temp
    NodePT nodeBuffer = new BasicNode;
    BlockPtr blk = root;
    blk->setoffset(0);
    blk->read(nodeBuffer, NODE_SIZE);
    while (0x1 & nodeBuffer->isLeaf) {
        stack.push_back(blk);
        if (Value::valcmp(GETVAL(nodeBuffer->roffset[nodeBuffer->size - 1]), v) <= 0) {
            // val <= v
            stackPos.push_back(nodeBuffer->size);
            blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->foffset[nodeBuffer->size]));
            READ_NODE_BUF(blk, nodeBuffer);
            continue;
        }
        for (int i = 0; i < nodeBuffer->size; i++) {
            if (Value::valcmp(GETVAL(nodeBuffer->roffset[i]), v) > 0) {
                // val > v
                stackPos.push_back(i);
                if (nodeBuffer->foffset[i] != 0) {
                    blk = bufferManager->getblock(MakeID(fileManager, nodeBuffer->foffset[i]));
                    // update node buffer
                    READ_NODE_BUF(blk, nodeBuffer);
                }
                break;
            }
        }
        return blk;
    }
}

unsigned long long BplusTree::findOne(Value v) {
    BlockPtr blk = findNode(v);
    auto nodeBuffer = new BasicNode;
    blk->setoffset(0);
    blk->read(nodeBuffer, NODE_SIZE);
    for (int i = 0; i < nodeBuffer->size; i++) {
        if (0 == Value::valcmp(GETVAL(nodeBuffer->roffset[i]), v)) {
            // return the offset(ptr) of the record
            return nodeBuffer->roffset[i];
        }
    }
    return 0;
}

BlockPtr BplusTree::findLeftMostNode() {
    BlockPtr blk = root;
    auto nodeBuffer = new BasicNode;
    READ_NODE_BUF(blk, nodeBuffer);
    while (!nodeBuffer->isLeaf) {
        blk = GET_IDX_BLOCK(nodeBuffer->foffset[0]);
        READ_NODE_BUF(blk, nodeBuffer);
    }
    return blk;
}

int BplusTree::findByRange(
        bool wl, bool leq, const Value &l,
        bool wr, bool req, const Value &r,
        const BpRangeConsumer &consumer
) {
    int count = 0;
    BlockPtr blk;
    auto nodeBuffer = new BasicNode;
    int now = 0;
    if (wl) {
        blk = findNode(l);
        READ_NODE_BUF(blk, nodeBuffer);
        bool find = false;
        // find? the lower bound
        for (int i = 0; i < nodeBuffer->size; i++) {
            if (Value::valcmp(GETVAL(nodeBuffer->roffset[i]), l) > 0
                || (Value::valcmp(GETVAL(nodeBuffer->roffset[i]), l) == 0 && leq)) {
                // val > l
                find = true;
                now = i;
                break;
            }
        }
        if (!find) {
            if (0 == NEXT_NODE(nodeBuffer)) return 0;
            blk = GET_IDX_BLOCK(NEXT_NODE(nodeBuffer));
            READ_NODE_BUF(blk, nodeBuffer);
        }
    } else { // !wl
        blk = findLeftMostNode();
        READ_NODE_BUF(blk, nodeBuffer);
    }
    while (true) {
        for (int i = now; i < nodeBuffer->size; i++) {
            if (wr && (Value::valcmp(GETVAL(nodeBuffer->roffset[i]), r) > 0
                       || (Value::valcmp(GETVAL(nodeBuffer->roffset[i]), r) == 0 && !req))) {
                return count;
            }
            consumer(count++, nodeBuffer->foffset[i]);
        }
    }
}

bool BplusTree::insert(unsigned long long roffset, unsigned long long foffset) {
    Value v = GETVAL(roffset);
    BlockPtr blk = findNode(v);
    auto nodeBuffer = new BasicNode;
    READ_NODE_BUF(blk, nodeBuffer);
    if (nodeBuffer->size < NODE_SIZE) {
        // the node is not full, just insert
        for (int i = nodeBuffer->size; i >= 1; i--) {
            if (Value::valcmp(GETVAL(nodeBuffer->roffset[i - 1]), v) < 0) {
                // val < v
                nodeBuffer->roffset[i] = roffset;
                nodeBuffer->foffset[i] = foffset;
                nodeBuffer->size++;
                // write back to block
                WRITE_BACK_NODE_BUF(blk, nodeBuffer);
                return true; // insertion complete
            } else if (0 == Value::valcmp(GETVAL(nodeBuffer->roffset[i - 1]), v)) {
                // TODO: remove
                std::cout << "[TEST][WARNING]: duplicate value encountered, ignoring" << std::endl;
                return false; // duplicate value
            } else {
                nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
            }
        }
        nodeBuffer->roffset[0] = roffset;
        nodeBuffer->foffset[0] = foffset;
        nodeBuffer->size++;
        // write back to block
        WRITE_BACK_NODE_BUF(blk, nodeBuffer);
    } else {
        // if there is a equal key
        for (int i = nodeBuffer->size; i >= 1; i--) {
            if (0 == Value::valcmp(GETVAL(nodeBuffer->roffset[i - 1]), v)) return false;
        }
        // split the node into [HALF_NODE_SIZE, HALF_NODE_SIZE]
        // then do a recursive insertion
        BlockPtr nblk = createNode();
        auto nNodeBuffer = new BasicNode;
        READ_NODE_BUF(nblk, nNodeBuffer);
        NEXT_NODE(nNodeBuffer) = NEXT_NODE(nodeBuffer);
        NEXT_NODE(nodeBuffer) = blk->blocknumber();
        nNodeBuffer->isLeaf = true;
        nNodeBuffer->size = 0;
        nodeBuffer->size = NODE_SIZE_HALF;
        if (Value::valcmp(GETVAL(nodeBuffer->roffset[NODE_SIZE_HALF - 1]), v) < 0) {
            // val < v
            // nodeBuffer do not requires further modifications -> write back to buffer
            bool flag = false; // weather the value is inserted
            for (int i = NODE_SIZE_HALF; i < NODE_SIZE; i++) {
                if (!flag && Value::valcmp(GETVAL(nodeBuffer->roffset[NODE_SIZE_HALF - 1]), v) < 0) {
                    // val < v
                    nNodeBuffer->roffset[nNodeBuffer->size] = roffset;
                    nNodeBuffer->foffset[nNodeBuffer->size++] = foffset;
                    flag = true;
                }
                nNodeBuffer->roffset[nNodeBuffer->size] = nodeBuffer->roffset[i];
                nNodeBuffer->foffset[nNodeBuffer->size++] = nodeBuffer->foffset[i];
            }
            if (!flag) {
                nNodeBuffer->roffset[nNodeBuffer->size] = roffset;
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
                if (Value::valcmp(GETVAL(nodeBuffer->roffset[i - 1]), v) < 0) {
                    // val < v
                    nodeBuffer->roffset[i] = roffset;
                    nodeBuffer->foffset[i] = foffset;
                    flag = true;
                    break;
                } else {
                    nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                    nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
                }
            }
            if (!flag) {
                nodeBuffer->roffset[0] = roffset;
                nodeBuffer->foffset[0] = foffset;
            }
        }
        // remember to write back nodeBuffer to blk
        WRITE_BACK_NODE_BUF(blk, nodeBuffer);
        WRITE_BACK_NODE_BUF(nblk, nNodeBuffer);
        stack.push_back(blk);
        insertInto(nNodeBuffer->roffset[0], nblk);// update the B+ tree from bottom up
    }
    // sure the data in write back to the buffer
    WRITE_BACK_NODE_BUF(blk, nodeBuffer);
    return true;
}

void BplusTree::insertInto(unsigned long long roffset, BlockPtr nenodeBuffertBlock) {
    if (now == -1) { // root
        BlockPtr blk = createNode();
        auto nodeBuffer = new BasicNode;
        auto rootNode = new BasicNode;
        READ_NODE_BUF(stack[0], nodeBuffer);
        blk->setdirty(true);

        //TODO: check the root
        READ_NODE_BUF(root, rootNode);
        rootNode->isLeaf = false;
        rootNode->size = 1;
        rootNode->foffset[0] = blk->blocknumber();
        rootNode->foffset[1] = nenodeBuffertBlock->blocknumber();
        rootNode->roffset[0] = roffset;
        // Write back root
        WRITE_BACK_NODE_BUF(root, rootNode);
    } else {
        BlockPtr blk = stack[now];
        NodePT nodeBuffer = new BasicNode;
        READ_NODE_BUF(blk, nodeBuffer);
        int pos = stackPos[now];
        // insert at (rofst[i], fofst[i + 1]) (i = pos)
        if (nodeBuffer->size < NODE_SIZE) {
            // the Node is not full just fo a simple insert
            for (int i = nodeBuffer->size; i > pos; i--) {
                // shift the values
                nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
            }
            nodeBuffer->roffset[pos] = roffset;
            nodeBuffer->foffset[pos + 1] = nenodeBuffertBlock->blocknumber();
            nodeBuffer->size++;
        } else {
            /* the node is full and needs splitting
             * When the target non-leaf node is full, split is required
             * unlike split the leaf-node, the left-most son ptr has no
             * indenodeBuffer value, so the result size is (HALF, HALF - 1)
             */
            BlockPtr nblk = createNode();
            auto nNodeBuffer = new BasicNode;
            unsigned long long parentValue;
            READ_NODE_BUF(nblk, nNodeBuffer);
            nNodeBuffer->isLeaf = false;
            nodeBuffer->size = NODE_SIZE_HALF;
            if (pos > NODE_SIZE_HALF) {
                // origin node is well-done
                // target node requires insert new value
                // the node spilt method is like following
                // v: [* * *] (*) [& *]   >>> throw the (*)
                // s:[0 1 2 3]   [4 & 5]  >>> find place to insert
                parentValue = nodeBuffer->roffset[NODE_SIZE_HALF];
                nNodeBuffer->foffset[0] = nodeBuffer->foffset[NODE_SIZE_HALF + 1];
                for (int i = NODE_SIZE_HALF + 1; i <= NODE_SIZE; i++) {
                    if (i == pos) {
                        nNodeBuffer->roffset[nNodeBuffer->size] = roffset;
                    }
                    if (i != NODE_SIZE) {
                        nNodeBuffer->roffset[nNodeBuffer->size] = nodeBuffer->roffset[i];
                        nNodeBuffer->foffset[++nNodeBuffer->size] = nodeBuffer->foffset[i + 1];
                    }
                }
            } else { // pos <= NODE_SIZE_HALF >>> just copy the values to target node
                nNodeBuffer->foffset[0] = nodeBuffer->foffset[NODE_SIZE_HALF];
                for (int i = NODE_SIZE_HALF; i < NODE_SIZE; i++) {
                    nNodeBuffer->roffset[nNodeBuffer->size++] = nodeBuffer->roffset[i];
                    nNodeBuffer->foffset[nNodeBuffer->size] = nodeBuffer->foffset[i + 1];
                }
                if (pos == NODE_SIZE_HALF) {
                    // do simple replacement
                    // v: [* * *] (&) [* *]   >>> throw the (&)
                    // s:[0 1 2 3]   [& 4 5]  >>> do the simple replacement
                    nNodeBuffer->foffset[0] = nenodeBuffertBlock->blocknumber();
                    parentValue = roffset;
                } else {
                    // origin node requires insertion
                    // v: [* * &] (*) [* *]   >>> throw the (*)
                    // s:[0 1 2 &]   [3 4 5]  >>> find place to insert
                    parentValue = nodeBuffer->roffset[NODE_SIZE_HALF - 1];
                    for (int i = NODE_SIZE_HALF - 1; i >= 0; i--) {
                        if (i == pos) {
                            nodeBuffer->foffset[i + 1] = nenodeBuffertBlock->blocknumber();
                            nodeBuffer->roffset[i] = roffset;
                            break;
                        }
                        nodeBuffer->foffset[i + 1] = nodeBuffer->foffset[i];
                        nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                    }
                }
            }
            WRITE_BACK_NODE_BUF(nblk, nNodeBuffer);
            WRITE_BACK_NODE_BUF(blk, nodeBuffer);
            now--; // do recursion
            insertInto(parentValue, nblk);
        }
    }
}

bool BplusTree::remove(Value v) {
    BlockPtr blk = findNode(v);
    NodePT nodeBuffer = new BasicNode;
    READ_NODE_BUF(blk, nodeBuffer);
    int pos;
    for (pos = 0; pos < nodeBuffer->size; pos++) {
        if (0 == Value::valcmp(GETVAL(nodeBuffer->roffset[pos]), v)) break;
    }
    if (pos == nodeBuffer->size) return false;

    for (int i = pos; i < nodeBuffer->size; i++) {
        nodeBuffer->roffset[i] = nodeBuffer->roffset[i + 1];
        nodeBuffer->foffset[i] = nodeBuffer->foffset[i + 1];
    }
    nodeBuffer->size--;
    WRITE_BACK_NODE_BUF(blk, nodeBuffer);
    if (blk->blocknumber() == root->blocknumber()) return true;
    BlockPtr pblk = stack[now];
    NodePT parentNode = new BasicNode;
    READ_NODE_BUF(pblk, parentNode);
    int parentPos = stackPos[now];

    if (nodeBuffer->size >= NODE_SIZE_HALF) {
        if (parentPos != 0) {
            parentNode->roffset[parentPos - 1] = nodeBuffer->roffset[0];
            WRITE_BACK_NODE_BUF(pblk, parentNode);
        }
        return true;
    }

    if (parentPos != 0) {
        BlockPtr sibblk = GET_IDX_BLOCK(parentNode->foffset[parentNode->size - 1]);
        NodePT sibNode = new BasicNode;
        READ_NODE_BUF(sibblk, sibNode);
        if (sibNode->size > NODE_SIZE_HALF) { // borrow
            for (int i = nodeBuffer->size; i >= 1; i--) {
                nodeBuffer->roffset[i] = nodeBuffer->roffset[i - 1];
                nodeBuffer->foffset[i] = nodeBuffer->foffset[i - 1];
            }
            nodeBuffer->roffset[0] = sibNode->roffset[sibNode->size - 1];
            nodeBuffer->foffset[0] = sibNode->foffset[sibNode->size - 1];
            parentNode->roffset[parentPos - 1] = nodeBuffer->roffset[0];
            sibNode->size--;
            nodeBuffer->size--;
        } else { // merge
            for (int i = 0; i < nodeBuffer->size; i++) {
                sibNode->roffset[sibNode->size + i] = nodeBuffer->roffset[i];
                sibNode->foffset[sibNode->size + i] = nodeBuffer->foffset[i];
            }
            sibNode->size += nodeBuffer->size;
            NEXT_NODE(sibNode) = NEXT_NODE(nodeBuffer);
            nodeBuffer->isLeaf = NODE_DELETED;
            removeForm();
        }
        WRITE_BACK_NODE_BUF(sibblk, sibNode);
    } else {
        BlockPtr sibblk = GET_IDX_BLOCK(parentNode->foffset[parentNode->size - 1]);
        NodePT sibNode = new BasicNode;
        READ_NODE_BUF(sibblk, sibNode);
        if (sibNode->size > NODE_SIZE_HALF) { // borrow
            nodeBuffer->roffset[nodeBuffer->size] = sibNode->roffset[0];
            nodeBuffer->foffset[nodeBuffer->size++] = sibNode->foffset[1];
            parentNode->roffset[parentPos] = nodeBuffer->roffset[0];
            for (int i = 0; i < sibNode->size - 1; i++) {
                nodeBuffer->roffset[i] = nodeBuffer->roffset[i + 1];
                nodeBuffer->foffset[i] = nodeBuffer->foffset[i + 1];
            }
            sibNode->size--;
        } else { // merge
            for (int i = 0; i < sibNode->size; i++) {
                nodeBuffer->roffset[nodeBuffer->size + i] = sibNode->roffset[i];
                nodeBuffer->foffset[nodeBuffer->size + i] = sibNode->foffset[i];
            }
            nodeBuffer->size += sibNode->size;
            NEXT_NODE(nodeBuffer) = NEXT_NODE(sibNode);
            sibNode->isLeaf = NODE_DELETED;
            removeForm();
        }
        WRITE_BACK_NODE_BUF(sibblk, sibNode);
    }
    WRITE_BACK_NODE_BUF(pblk, parentNode);
    return true;
}

void BplusTree::removeForm() {
    BlockPtr blk = stack[now];
    NodePT nodeBuffer = new BasicNode;
    READ_NODE_BUF(blk, nodeBuffer);
    int pos = stackPos[now];
    for (int i = pos == 0 ? 0 : pos - 1; i < nodeBuffer->size; i++) {
        nodeBuffer->roffset[i] = nodeBuffer->roffset[i + 1];
        nodeBuffer->foffset[i + 1] = nodeBuffer->foffset[i + 2];
    }
    WRITE_BACK_NODE_BUF(blk, nodeBuffer);
    nodeBuffer->size--;
    if (now == 0) { // root
        NodePT rootNode = new BasicNode;
        READ_NODE_BUF(root, rootNode);
        if (!rootNode->isLeaf && rootNode->size == 0) {
            root->setfree(true);
            root->setdirty(true);
        }
    } else {
        BlockPtr pblk = stack[now - 1];
        NodePT parentNode = new BasicNode;
        int parentPos = stackPos[now - 1];
        READ_NODE_BUF(pblk, parentNode);
        if (nodeBuffer->size < NODE_SIZE_HALF - 1) {
            if (parentPos != 0) {
                BlockPtr sibblk = GET_IDX_BLOCK(parentNode->foffset[parentPos - 1]);
                NodePT sibNode = new BasicNode;
                READ_NODE_BUF(sibblk, sibNode);
                if (sibNode->size > NODE_SIZE_HALF) { // borrow
                    for (int i = nodeBuffer->size; i >= 1; i--) {
                        nodeBuffer->roffset[i], nodeBuffer->roffset[i - 1];
                        nodeBuffer->foffset[i + 1] = nodeBuffer->foffset[i];
                    }
                    nodeBuffer->foffset[1] = nodeBuffer->foffset[0];
                    nodeBuffer->roffset[0] = parentNode->roffset[parentPos - 1];
                    parentNode->roffset[parentPos - 1] = sibNode->roffset[sibNode->size - 1];
                    nodeBuffer->foffset[0] = sibNode->foffset[sibNode->size];
                    nodeBuffer->size++;
                    sibNode->size--;
                } else { //merge
                    sibNode->roffset[sibNode->size], parentNode->roffset[parentPos - 1];
                    sibNode->foffset[sibNode->size + 1] = nodeBuffer->foffset[0];
                    for (int i = 0; i < nodeBuffer->size; i++) {
                        sibNode->roffset[sibNode->size + i + 1] = nodeBuffer->roffset[i];
                        sibNode->foffset[sibNode->size + i + 2] = nodeBuffer->foffset[i + 1];
                    }
                    sibNode->size += nodeBuffer->size + 1;
                    nodeBuffer->isLeaf = NODE_DELETED;
                    now--;
                    removeForm();
                }
                WRITE_BACK_NODE_BUF(sibblk, sibNode);
            } else {
                BlockPtr sibblk = GET_IDX_BLOCK(parentNode->foffset[parentPos + 1]);
                NodePT sibNode = new BasicNode;
                READ_NODE_BUF(sibblk, sibNode);
                if (sibNode->size > NODE_SIZE_HALF) { // borrow
                    nodeBuffer->foffset[nodeBuffer->size + 1] = sibNode->foffset[0];
                    nodeBuffer->roffset[nodeBuffer->size] = parentNode->roffset[parentPos];
                    parentNode->roffset[parentPos] = sibNode->roffset[0];
                    nodeBuffer->foffset[0] = sibNode->foffset[1];
                    for (int i = sibNode->size; i >= 1; i--) {
                        sibNode->roffset[i] = sibNode->roffset[i + 1];
                        sibNode->foffset[i + 1] = sibNode->foffset[i + 2];
                    }
                    sibNode->size--;
                    nodeBuffer->size++;
                } else { //merge
                    nodeBuffer->roffset[nodeBuffer->size], parentNode->roffset[parentPos];
                    nodeBuffer->foffset[nodeBuffer->size + 1] = sibNode->foffset[0];
                    for (int i = 0; i < sibNode->size; i++) {
                        nodeBuffer->roffset[nodeBuffer->size + i + 1] = sibNode->roffset[i];
                        nodeBuffer->foffset[nodeBuffer->size + i + 2] = sibNode->foffset[i + 1];
                    }
                    nodeBuffer->size += sibNode->size + 1;
                    sibNode->isLeaf = NODE_DELETED;
                    now--;
                    removeForm();
                }
                WRITE_BACK_NODE_BUF(sibblk, sibNode);
            }
        }
        WRITE_BACK_NODE_BUF(pblk, parentNode);
    }

}