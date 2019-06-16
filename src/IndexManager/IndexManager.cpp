#include <utility>
//
// Created by Henry Little on 2019-06-08.
//

#include "IndexManager.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const unsigned long long &pos) { return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE); };

IndexManager::IndexManager(const BufferPtr &buffer, const std::string &indexname, const SchemaPtr &schema,
                           const std::string colName, IndexHeader header) {
    this->buffer = buffer;
    this->columnName = colName;
    this->schema = schema;
    this->header = header;
    if (RecordManager::recordexist(schema->header.name))
        this->recordManager = RecordManager::recordbuf[schema->header.name];
    else {
        this->recordManager = std::make_shared<RecordManager>(buffer, schema);
        RecordManager::recordbuf[schema->header.name] = this->recordManager;
    }

    std::string filename = FILENAME_PREFIX + indexname + "-indexon-" + colName + "-of-" + schema->header.name;

    if (FileManager::filexist(filename.c_str())) {
        this->indexFile = std::make_shared<FileManager>(filename.c_str());
        this->bplusTree = std::make_shared<BplusTree>(
                indexFile, buffer, recordManager,
                buffer->getblock(MakeID(indexFile, header.treeRoot)),
                columnName, false
        );
    } else {
        FileManager::createfile(filename.c_str());
        this->indexFile = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = indexFile;
        this->bplusTree = std::make_shared<BplusTree>(
                indexFile, buffer, recordManager,
                buffer->getblock(MakeID(indexFile, indexFile->allocblock())),
                columnName, true
        );
    }

}


void IndexManager::buildIndex() {
    BlockPtr tmpblk;
    int count = 0;
    unsigned long long ptr = recordManager->header.recordstart; // equal to foffset
    while (ptr) {
        if (ptr & DELETE_TAG) {
            ptr &= DELETE_MASK;
            // skip a delete record (current record)
        } else {
            //std::cout << count++ << " Inserting ptr: " << ptr << std::endl;
            bplusTree->insert(ptr + sizeof(unsigned long long) + schema->name2offset[std::string(columnName)], ptr);
        }
        // read the next ptr to read
        FilePos pos = CalcFilePos(ptr);
        tmpblk = buffer->getblock(MakeID(recordManager->file, pos.first));
        tmpblk->setoffset(pos.second);
        tmpblk->read(&ptr, sizeof(unsigned long long));
        //std::cout << "Next Ptr: " << ptr << std::endl;
    }
    // store the root of the tree
    this->header.treeRoot = bplusTree->getRootNum();
}

void IndexManager::insertOne(unsigned long long roffset, unsigned long long foffset) {
    bplusTree->insert(roffset, foffset);
}

unsigned long long IndexManager::findOne(Value &v) {
    return bplusTree->findOne(v); // roffset
}

std::vector<unsigned long long> IndexManager::findByRange(
        bool wl, bool leq, const Value &l,
        bool wr, bool req, const Value &r
) {
    std::vector<unsigned long long> res;
    bplusTree->findByRange(
            wl, leq, l,
            wr, req, r,
            [&res](int count, unsigned long long ptr) {
                res.push_back(ptr);
            });
    return res;
}

void IndexManager::removeOne(const Value &v) {
    bplusTree->remove(v);
}

void IndexManager::deleter(Value &value) {
    removeOne(value);
}

void IndexManager::printRecord(unsigned long long ptr) {
    FilePos pos = CalcFilePos(ptr);
    BlockPtr blk = buffer->getblock(MakeID(recordManager->file, pos.first));
    blk->setoffset(pos.second + sizeof(unsigned long long));
    std::cout << "Reading record at: " << ptr << " >>> ";
    for (auto &a : schema->attrs) {
        void *ppt;
        if (a->vtype == Type::CHAR) {
            ppt = new char[a->clen];
            blk->read(ppt, a->size());
            std::cout << (char *) ppt << " ";
        } else if (a->vtype == Type::FLOAT) {
            ppt = new float;
            blk->read(ppt, a->size());
            std::cout << *(float *) ppt << " ";
        } else {
            ppt = new int;
            blk->read(ppt, a->size());
            std::cout << *(int *) ppt << " ";
        }
    }
    std::cout << std::endl;
}

std::vector<Record> IndexManager::translateAndFilter(
        const std::vector<unsigned long long> &results,
        const Limits &limits
) {
    std::vector<Record> retval;
    for (unsigned long long result : results) {
        FilePos pos = CalcFilePos(result);
        BlockPtr blk = buffer->getblock(MakeID(recordManager->file, pos.first));
        blk->setoffset(pos.second + sizeof(unsigned long long));
        Record record;
        for (auto &a : schema->attrs) {
            Value val;
            if (a->vtype == Type::CHAR) {
                val.type = Type::CHAR;
                val.ptr = new char[a->clen];
                blk->read(val.ptr, a->size());
            } else if (a->vtype == Type::FLOAT) {
                val.type = Type::FLOAT;
                val.ptr = new float;
                blk->read(val.ptr, a->size());
            } else {
                val.type = Type::INT;
                val.ptr = new int;
                blk->read(val.ptr, a->size());
            }
            record.emplace_back(std::move(val));
        }
        bool flag = false;
        for (auto &lim: limits) {
            flag &= Constraint::valcmp(lim.op, record[lim.attridx], lim.val);
            if (!flag) break;
        }
        retval.emplace_back(std::move(record));
    }
    return retval;
}
