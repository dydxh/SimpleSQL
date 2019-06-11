#include <utility>

#include <utility>

//
// Created by Henry Little on 2019-06-08.
//

#include "IndexManager.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const unsigned long long &pos) { return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE); };

IndexManager::IndexManager(const BufferPtr &buffer, const SchemaPtr &schema, const std::string colName) {
    this->buffer = buffer;
    this->columnName = colName;
    this->schema = schema;
    this->recordManager = RecordManager::recordbuf[schema->header.name];

    std::string filename = std::string(FILENAME_PREFIX) + "index-" + colName + "-" + std::string(schema->header.name);

    if (FileManager::filexist(filename.c_str())) {
        indexFile = std::make_shared<FileManager>(filename.c_str());
    } else {
        FileManager::createfile(filename.c_str());
        this->indexFile = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = indexFile;
        // assert(indexFile->allocblock() == 0);
        // TODO: write the file header
    }
    this->bplusTree = std::make_shared<BplusTree>(
            indexFile, buffer, recordManager,
            buffer->getblock(MakeID(indexFile, indexFile->allocblock())),
            columnName
    );

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
}

unsigned long long IndexManager::findOne(Value &v) {
    unsigned long long ptr = bplusTree->findOne(v);
    //TODO:remove
    this->printRecord(ptr - sizeof(unsigned long long) - schema->name2offset[columnName]);
    return ptr;
}

std::vector<unsigned long long> IndexManager::findByRange(
        bool wl, bool leq, const Value &l,
        bool wr, bool req, const Value &r
) {
    std::vector<unsigned long long> res;
    bplusTree->findByRange(
            wl, leq, l,
            wr, req, r,
            [&res, this](int count, unsigned long long ptr) {
                res.push_back(ptr);
                //TODO:remove
                std::cout << count << " >>> ";
                printRecord(ptr);
            });
    return res;
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

