//
// Created by Henry Little on 2019-06-08.
//

#include "IndexManager.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const unsigned long long& pos) {return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE);};

IndexManager::IndexManager(const BufferPtr &recordBuffer, const SchemaPtr &schema, const std::string colName) {
    this->buffer = buffer;
    this->columnName = colName;
    this->schema = schema;
    this->recordManager = std::shared_ptr(RecordManager::recordbuf[schema->header.name]);

    std::string filename = std::string(FILENAME_PREFIX) + "index-" + colName + "-" + std::string(schema->header.name);

    if (FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        this->indexFile = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = indexFile;
        assert(indexFile->allocblock() == 0);
        // TODO: write the file header
    } else {
        if(FileManager::filebuf[filename].expired() == false) {
            indexFile = FileManager::filebuf[filename].lock();
        } else {
            indexFile = std::make_shared<FileManager>(filename.c_str()); // make_shared will call the constructor of the FileManger
            FileManager::filebuf[filename] = indexFile;
        }
    }
    this->bplusTree = std::make_shared<BplusTree>(indexFile, buffer, recordManager, 
    buffer->getblock(MakeID(indexFile, indexFile->allocblock)), columnName);
    
}



void IndexManager::buildIndex() {
    BlockPtr tmpblk;
    unsigned long long ptr = recordManager->header.recordstart; // equal to foffset
    while(ptr) {
        FilePos pos = CalcFilePos(ptr);
        tmpblk = buffer->getblock(MakeID(recordFile, pos.first));
        tmpblk->setoffset(pos.second);
        tmpblk->read(&ptr, sizeof(unsigned long long));
        if(ptr & DELETE_TAG) {
            ptr &= DELETE_MASK;
            continue; // skip a delete record
        }
        bplusTree->insert(ptr + schema->name2offset[std::string(columnName)], ptr);
    }
}